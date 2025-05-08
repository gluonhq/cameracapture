package com.gluonhq.cameracapture;

import static java.lang.foreign.ValueLayout.ADDRESS;
import static java.lang.foreign.ValueLayout.JAVA_BYTE;
import static java.lang.foreign.ValueLayout.JAVA_INT;
import static java.lang.foreign.ValueLayout.JAVA_LONG;

import java.lang.foreign.Arena;
import java.lang.foreign.FunctionDescriptor;
import java.lang.foreign.Linker;
import java.lang.foreign.MemoryLayout;
import java.lang.foreign.MemorySegment;
import java.lang.invoke.MethodHandle;
import java.lang.invoke.MethodHandles;
import java.lang.invoke.MethodType;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.function.Consumer;

import com.gluonhq.cameracapture.callback.InnerFrameCallback;
import com.gluonhq.cameracapture.h.NativeCapture;

public class CameraCapture {

    private static InnerFrameCallback callback;

    private AtomicBoolean listening = new AtomicBoolean(false);
    private Consumer<Frame> clientCallback;

    static {
        try {
			FunctionDescriptor gotNativeFrameDescriptor = FunctionDescriptor.ofVoid(JAVA_INT, JAVA_INT, JAVA_INT, ADDRESS.withTargetLayout(MemoryLayout.sequenceLayout(Long.MAX_VALUE, JAVA_BYTE)), JAVA_INT);
			MethodHandle gotNativeFrameHandle = MethodHandles.lookup().findStatic(
				CameraCapture.class, "gotNativeFrame",
                    MethodType.methodType(void.class, int.class, int.class, int.class, MemorySegment.class, int.class)
			);
			System.out.println("DESCRIPTOR: " + gotNativeFrameDescriptor);
			System.out.println("HANDLE: " + gotNativeFrameHandle);
			MemorySegment gotNativeFrameSegment = Linker.nativeLinker().upcallStub(gotNativeFrameHandle, gotNativeFrameDescriptor, Arena.global());
			NativeCapture.got_frame(gotNativeFrameSegment);
		} catch (Exception e) {
			e.printStackTrace();
		}
    }

    private static void gotNativeFrame(int width, int height, int format, MemorySegment memory, int length) {
        if (callback != null) {
			byte[] pixels = new byte[length];
			MemorySegment.copy(memory, JAVA_BYTE, 0, pixels, 0, length);
            callback.gotFrame(width, height, format, pixels);
        }
    }

    public CameraCapture() {
        callback = this::gotFrame;

        NativeCapture.init();
    }

    /**
     * Called by clients to indicate they are ready to process incoming frames
     * from the camera. Whenever a frame is ready, the provided callback will
     * be invoked.
     *
     * @param callback a Consumer that will tell the client we have a frame
     */
    public boolean startListening(Consumer<Frame> callback) {
        boolean started = listening.compareAndSet(false, true);
        if (started) {
            this.clientCallback = callback;
            NativeCapture.start();
        }
        return started;
    }

    /**
     * Called by clients to indicate that they no longer wish to receive
     * frame updates.
     */
    public void stopListening() {
        if (listening.compareAndSet(true, false)) {
            NativeCapture.stop();
        }
    }

    private void gotFrame(int width, int height, int format, byte[] data) {
        this.clientCallback.accept(new Frame(width, height, format, data));
    }
}
