#include "core/network/WebSocketClient.h"

#include <QAbstractSocket>

namespace NeriPlayerQt {

WebSocketClient::WebSocketClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_socket, &QWebSocket::connected, this, &WebSocketClient::connected);
    connect(&m_socket, &QWebSocket::disconnected, this, &WebSocketClient::disconnected);
    connect(&m_socket, &QWebSocket::textMessageReceived, this, &WebSocketClient::textMessageReceived);
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    connect(&m_socket, &QWebSocket::errorOccurred, this,
            [this](QAbstractSocket::SocketError) { emit errorOccurred(m_socket.errorString()); });
#else
    connect(
        &m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this,
        [this](QAbstractSocket::SocketError) { emit errorOccurred(m_socket.errorString()); });
#endif
}

void WebSocketClient::connectTo(const QUrl &url)
{
    m_socket.open(url);
}

void WebSocketClient::sendTextMessage(const QString &message)
{
    if (isConnected()) {
        m_socket.sendTextMessage(message);
    }
}

void WebSocketClient::close()
{
    m_socket.close();
}

bool WebSocketClient::isConnected() const
{
    return m_socket.state() == QAbstractSocket::ConnectedState;
}

} // namespace NeriPlayerQt
