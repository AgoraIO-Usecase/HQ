package agora.io.agorahq;

import android.util.Log;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


public class VMExceptionHandler implements Thread.UncaughtExceptionHandler {
    final static Logger logger = LoggerFactory.getLogger(VMExceptionHandler.class);
    private Thread.UncaughtExceptionHandler exceptionHandler;

    private VMExceptionHandler(Thread.UncaughtExceptionHandler originalHandler) {
        this.exceptionHandler = originalHandler;
    }

    public static void install() {
        Thread.UncaughtExceptionHandler currentHandler = Thread.getDefaultUncaughtExceptionHandler();

        if (currentHandler instanceof VMExceptionHandler) {
            currentHandler = ((VMExceptionHandler)currentHandler).exceptionHandler;
        }

        Thread.setDefaultUncaughtExceptionHandler(new VMExceptionHandler(currentHandler));
    }

    @Override
    public void uncaughtException(Thread thread, Throwable e) {
        StringBuilder builder = new StringBuilder();
        builder.append("FATAL EXCEPTION: ").append(thread.getName()).append(" ").append(thread.getId()).append("\n");

        final String processName = Process.class.getName();
        if (processName != null)
            builder.append("Process: ").append(processName).append(", ");

        builder.append("PID: ").append(android.os.Process.myPid());
        builder.append('\n');

        builder.append(Log.getStackTraceString(e));

        String message = builder.toString();

        logger.error(message);

        if (exceptionHandler != null) {
            exceptionHandler.uncaughtException(thread, e);
        }
    }
}
