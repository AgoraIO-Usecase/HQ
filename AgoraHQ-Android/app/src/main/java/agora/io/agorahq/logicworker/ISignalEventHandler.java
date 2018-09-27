package agora.io.agorahq.logicworker;

public interface ISignalEventHandler {
    /**
     * signaling login success
     */
    void onLoginSuccess();

    /**
     * signaling error
     * @param error error code
     */
    void onError(int error);

    /**
     * received channel message
     * @param channel message form which channel
     * @param msgId current message id
     * @param msg msg content
     */
    void onChannelMessageReceived(final String channel, final long msgId, final String msg);

    /**
     * received message from person, we called it p2p
     * @param account
     * @param msgId
     * @param msg
     */
    void onMessageReceivedFrom(final String account, final long msgId, final String msg);
}
