
#include <time.h>
#include <stdlib.h>
#include <windows.h>

 #include "../mqttsn_transport.h"
 #include "../mqttsn_device.h"
#include "../mqttx_client.h"
#include "../mqttsn_defines.h"
#include "../mqttsn_client.h"
#include "../mqttsn_transport_dummy.h"
#include "../mqttsn_gateway.h"


class SnUdpTransport : public MQTTSNTransport {
    public:
        SnUdpTransport(uint32_t ip, uint16_t port);
        
        virtual uint16_t write_packet(const void * data, uint16_t data_len, MQTTSNAddress * dest);
        virtual int32_t read_packet(void * data, uint16_t data_len, MQTTSNAddress * src);
        virtual uint16_t broadcast(const void * data, uint16_t data_len);
    
    protected:
        int sock;
};

SnUdpTransport::SnUdpTransport(uint32_t ip, uint16_t port)
{
    sock = 1;
}

uint16_t SnUdpTransport::write_packet(const void * data, uint16_t data_len, MQTTSNAddress * dest)
{
    return data_len;
}
int32_t SnUdpTransport::read_packet(void * data, uint16_t data_len, MQTTSNAddress * src)
{
    return data_len;
}
uint16_t SnUdpTransport::broadcast(const void * data, uint16_t data_len)
{
    return data_len;
}

class SnWinDevice : public MQTTSNDevice {
    public:
        SnWinDevice();
        
        virtual uint32_t get_millis(void);
        virtual void cede(void);
        virtual void delay_millis(uint32_t ms);
        virtual uint32_t get_random(uint32_t min, uint32_t max);
};

SnWinDevice::SnWinDevice()
{
}

uint32_t SnWinDevice::get_millis(void) 
{
    return (uint32_t)clock();
}
void SnWinDevice::cede(void)
{
    
}
void SnWinDevice::delay_millis(uint32_t ms)
{
    Sleep(ms);
}
uint32_t SnWinDevice::get_random(uint32_t min, uint32_t max)
{
    return (uint32_t)(rand()%(max-min)+min);
}


void publish_callback(const char * topic, uint8_t * data, uint16_t len, MQTTSNFlags * flags)
{
    printf("\r\nTopic: %s\r\n", topic);
    printf("Len: %4d, Payload: ", len);

    for (int i = 0; i < len; i++) {
        printf("%x ", data[i]);
    }
}


class SnClient : public MQTTSNClient, public MQTTClient
{
public:
    SnClient(MQTTSNDevice * device, MQTTSNTransport * transport);
    //bool begin(const char * clnt_id);
    //bool connect(uint8_t gw_id = 0, MQTTSNFlags * flags = NULL, uint16_t duration = MQTTSN_DEFAULT_KEEPALIVE);
    // void on_message(MQTTSNPublishCallback callback);

    virtual void register_callbacks(void* self, MQTTClientConnectCallback conn_cb, MQTTClientMessageCallback msg_cb);
    virtual void publish(const char* topic, uint8_t* payload, uint16_t length, MQTTSNFlags* flags);
    virtual void subscribe(const char* topic, uint8_t qos);
    virtual void unsubscribe(const char* topic);

};

SnClient::SnClient(MQTTSNDevice * device, MQTTSNTransport * transport)
    : MQTTSNClient(device, transport)
{
    
}
void SnClient::register_callbacks(void* self, MQTTClientConnectCallback conn_cb, MQTTClientMessageCallback msg_cb)
{

}
void SnClient::publish(const char* topic, uint8_t* payload, uint16_t length, MQTTSNFlags* flags)
{

}
void SnClient::subscribe(const char* topic, uint8_t qos)
{

}
void SnClient::unsubscribe(const char* topic)
{

}

int main(int argc, char* argv[])
{
    int ret = 0;
    MQTTSNTransportDummy dummy1(1);
    MQTTSNTransportDummy dummy2(2);
    SnWinDevice snWinDev;
    SnClient client(&snWinDev, &dummy1);
    SnClient mqttcGw(&snWinDev, &dummy2);
    MQTTSNDevice* snd = &snWinDev;
    MQTTClient* snc = &mqttcGw;
    MQTTSNGateway gateway(snd, snc);
    
    // SnUdpTransport udpTrans(0x7f000001, 1883);
    // SnClient client(&snWinDev, &udpTrans);
    
    client.begin("SnClient");
    gateway.begin(2);
    gateway.register_transport(&dummy2);

    MQTTSNAddress gw_addr = { {2}, 1 };
    MQTTSNGWInfo gatewayInfo = { 2, gw_addr };
    client.add_gateways(&gatewayInfo, 1);
    client.on_message(publish_callback);

    client.connect(2);
    
    while (true) {
        gateway.loop();
        mqttcGw.loop();
        client.loop();
    }

    /* async-connect to first available gateway */
    if (!client.connect()) {
        Sleep(10);
    }
    
    return ret;
}