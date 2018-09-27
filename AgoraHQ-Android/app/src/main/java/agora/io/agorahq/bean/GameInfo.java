package agora.io.agorahq.bean;

import java.util.concurrent.ConcurrentHashMap;

public class GameInfo {
    //** current game state
    private PlayState playState = PlayState.NONE;

    //** only one question should we hold
    private ConcurrentHashMap<Integer, Question> questionMap = new ConcurrentHashMap<>();

    //** only one user should we hold
    private ConcurrentHashMap<Integer, User> userMap = new ConcurrentHashMap<>();

    //** only one user should we hold
    private ConcurrentHashMap<Integer, Answer> userAnswer = new ConcurrentHashMap<>();

    private boolean remotePlayStatus = true;
    private boolean localPlayStatus = true;

    private static GameInfo mGameInfo;
    private GameInfo(){}

    public static GameInfo newInstance() {
        if (mGameInfo == null) {
            synchronized (GameInfo.class) {
                if (mGameInfo == null) {
                    mGameInfo = new GameInfo();
                }
            }
        }

        return mGameInfo;
    }


    public void updateQuestion(Question question) {
        if (null == question) {
            questionMap.clear();
            return;
        }
        questionMap.put(0, question);
    }

    public Question getQuestion() {
        return questionMap.isEmpty() ? null : questionMap.get(0);
    }

    public void clearQuestion() {
        questionMap.clear();
    }

    public void updateUser(User user) {
        userMap.put(0, user);
    }

    public User getUser() {
        return userMap.isEmpty() ? null : userMap.get(0);
    }

    public void clearUser() {
        userMap.clear();
    }

    public void updatePlayState(PlayState ps) {
        playState = ps;
    }

    public PlayState getPlayState() {
        return playState;
    }

    public void resetPlayState() {
        playState = PlayState.NONE;
    }

    public boolean getRemotePlayStatus() {
        return remotePlayStatus;
    }

    public boolean getLocalPlayStatus() {
        return localPlayStatus;
    }

    public void updateLocalPlayStatus(boolean localPlayStatus) {
        this.localPlayStatus = localPlayStatus;
    }

    public void setRemotePlayStatus(boolean remotePlayStatus) {
        this.remotePlayStatus = remotePlayStatus;
    }

    public void updateAnswer(Answer answer) {
        if (null == answer) {
            userAnswer.clear();
            return;
        }
        userAnswer.put(0, answer);
    }

    public Answer getAnswer() {
        return userAnswer.isEmpty() ? null : userAnswer.get(0);
    }

    public void clearAnswer() {
        userAnswer.clear();
    }

    public void reset() {
        clearUser();
        clearQuestion();
        playState = PlayState.NONE;
    }
}
