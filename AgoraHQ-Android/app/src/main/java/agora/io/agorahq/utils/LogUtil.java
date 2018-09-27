package agora.io.agorahq.utils;

import android.os.Environment;
import android.util.Log;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

import agora.io.agorahq.AppBuildConfig;
import agora.io.agorahq.BuildConfig;

public class LogUtil {
	private String basePath = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "agroa_hq/";
	private static LogUtil writer;

	private LogUtil(){
		contentBuffer = new StringBuffer();
	};
	
	private StringBuffer contentBuffer;
	public static LogUtil newInstance(){
		if (writer == null) {
			synchronized (LogUtil.class) {
				if (writer == null) {
					writer = new LogUtil();
				}
			}
		}
		
		return writer;
	}
	
	public synchronized void log(String content){
		if (!AppBuildConfig.RUN_LOG_DEBUG || null == content || "".equals(content))
			return;

		File file;
		BufferedOutputStream bos;
		try {
			file = createCurrentDateFile();
			bos = new BufferedOutputStream(new FileOutputStream(file, true));
			contentBuffer.append(getCurrentTime());
			contentBuffer.append("\t\t");
			contentBuffer.append(content);
			printToScreen(contentBuffer.toString());
			contentBuffer.append("\n");

			bos.write(contentBuffer.toString().getBytes());
			Log.e("wbstest-->" , contentBuffer.toString());
			bos.flush();
			bos.close();
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			contentBuffer.delete(0, contentBuffer.length());
		}
	}
	
	// TODO create log file if it not existed
	private File createCurrentDateFile() throws IOException{
		SimpleDateFormat dateFormat = new SimpleDateFormat("yyyyMMdd");
		String fileName = dateFormat.format(new Date());
		File base = new File(basePath);
		if (!base.exists()){
			base.mkdirs();
		}

		File nowFile = new File(basePath, fileName + ".log");

		if (!nowFile.exists()){
			nowFile.createNewFile();
		}
		
		return nowFile;
	}
	
	private String getCurrentTime(){
		SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		return dateFormat.format(new Date());
	}
	
	private void printToScreen(String content){
		System.out.println("wbsTest-->\t" + content);
	}
}
