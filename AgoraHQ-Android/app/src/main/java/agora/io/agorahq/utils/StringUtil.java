package agora.io.agorahq.utils;

import java.util.Random;

public class StringUtil {

    public static String genRandomName(int length) {
        String str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        str += str.toLowerCase();

        if (length < 1)
            return null;

        StringBuilder strBuilder = new StringBuilder();
        Random random = new Random();

        for (int i = 0; i < length; i++) {
            strBuilder.append(str.charAt(random.nextInt(52)));
        }

        return strBuilder.toString();
    }
}
