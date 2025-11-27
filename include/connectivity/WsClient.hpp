#pragma once
#include "Protocol.hpp"
#include <WebSocketsClient.h>

class WsClient {
  public:
    WsClient(const char *host, uint16_t port, const char *path);

    void begin();
    void loop();
    void sendTelemetry(const Telemetry &t);

  private:
    void onWsEvent(WStype_t type, uint8_t *payload, size_t len);
    void handleCommand(const uint8_t *data, size_t len);

  private:
    WebSocketsClient client_;
    const char *host_;
    uint16_t port_;
    const char *path_;
};
