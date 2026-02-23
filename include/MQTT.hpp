#pragma once

#include <QNEthernet.h>
#include <algorithm>
#include "config.hpp"
#include <PubSubClient.h>
#include "LightLambda.hpp"

using namespace qindesign;

namespace MQTT {

#ifndef MQTT_MSUBS_MAX_DTYPE
    #define MQTT_MSUBS_MAX_DTYPE uint8_t
#endif // !MQTT_MSUBS_MAX_DTYPE

// struct SubscriberOnUpdate {
//     void* context;
//     void(* callback)(void*, uint8_t*, size_t);
// };

typedef Lambda<void, uint8_t*, size_t> SubscriberOnUpdate;

/// @brief Initializes the system network stack
/// @param self_hostname the system hostname
void initNetwork(const char* self_hostname) noexcept {
    bool stat = false;
    stat = stat && network::Ethernet.setDHCPEnabled(true);
    network::Ethernet.setHostname(self_hostname);
    stat = stat && network::Ethernet.begin();
}

/// @brief Resolve a server using DNS/mDNS
/// @param out_addr the output resolved IPAddress
/// @param server_hostname the server name to resolve
/// @param infinite_retry weather or not to indefinetly retry to resolve.
/// @return True if successfully resolved, false if not.
bool resolveServer(IPAddress &out_addr, const char* server_hostname, const bool infinite_retry = true) noexcept {
    bool is_resolved = false;
    IPAddress addr;
    do {
        is_resolved = network::Ethernet.hostByName(server_hostname, addr);
    } while (!is_resolved && infinite_retry);

    if (!is_resolved) {
        return false;
    }

    out_addr = addr;
    return true;
}

template<MQTT_MSUBS_MAX_DTYPE MSubs>
class Client {
public:
    class Publisher;
    class Subscriber;

    /// @brief Create a new MQTT client
    /// @param name [lifetime object] this client name 
    /// @param server_addr the server IPAddress
    /// @param server_port the server port
    /// @param auto_reconnect weather or not to auto-reconnect on disconnect
    /// @param auto_reconnect_delay_ms the delay in ms between each reconnect attempt
    Client(const char* name, 
        const IPAddress server_addr, const uint16_t server_port = 1883, 
        const bool auto_reconnect = true, const uint16_t auto_reconnect_delay_ms = 1000) noexcept
    : 
        m_auto_reconnect(auto_reconnect), m_auto_reconnect_delay(auto_reconnect_delay_ms)
    {
        network::EthernetClient eth; 
        this->m_client = PubSubClient { eth };
        this->m_name = name;

        m_client.setServer(server_addr, server_port);
    
        m_client.setCallback({ this, this->on_msg_receive });
    }
    /// @brief disconnects this client
    ~Client() noexcept {
        m_client.disconnect();
    }

    /// @brief Publish value to topic
    /// @param topic the topic path
    /// @param payload the payload
    /// @param size the payload size
    /// @param retained = true -- weather or not the published value is retained by the MQTT broker
    /// @return 
    bool publish(const char* topic, const uint8_t* payload, size_t size, bool retained = true) noexcept {
        return m_client.publish(topic, payload, size, retained);
    }

    /// @brief Subscribe to a topic
    /// @param topic the topic name
    /// @param callback the callback lambda
    /// @return True if succeffuly subscribed, false if not
    bool subscribe(const char* topic, SubscriberOnUpdate callback) noexcept {
        if (m_sub_map_size == MSubs) return false;

        if (!m_client.subscribe(topic)) return false;

        m_sub_map_topics[m_sub_map_size] = topic;
        m_sub_map_callback[m_sub_map_size] = callback;
        m_sub_map_size += 1;
        
        return true;
    }

    /// @brief Unsubscribe from a topic.
    /// @param topic the topic name
    /// @return True if successully unsubscribed, false if not
    bool unsuscribe(const char* topic) noexcept {
        if (m_sub_map_size == 0) return false;
        if (!m_client.unsubscribe(topic)) return false;

        bool is_found = false;
        MQTT_MSUBS_MAX_DTYPE idx;
        for (int i = 0; i < m_sub_map_size; i++) {
            if (strcmp(m_sub_map_topics[i], topic) == 0) {
                idx = i;
                is_found = true;
                break;
            }
        }
        if (!is_found) return false;
        // shift array elenments left
        memmove(m_sub_map_callback + idx, m_sub_map_callback + idx + 1, sizeof(SubscriberOnUpdate) * (m_sub_map_size - idx));
        memmove(m_sub_map_topics + idx, m_sub_map_topics + idx + 1, sizeof(const char*) * (m_sub_map_size - idx));
        m_sub_map_size -= 1;
        return true;
    }

    /// @brief Update this MQTT client, this is to be repeatly called in loop() 
    void update() noexcept {
        while (m_auto_reconnect && !m_client.connected()) {
            delay(m_auto_reconnect_delay);
            m_client.connect(m_name);
        }
        m_client.loop();
    }
private:
    static void on_msg_receive(void* this_ref, char* topic, uint8_t* payload, size_t length) noexcept {
        Client* self = static_cast<Client*>(this_ref);
        for (int i = 0; i < self->m_sub_map_size; i++) {
            if (strcmp(self->m_sub_map_topics[i], topic) != 0) continue;

            self->m_sub_map_callback[i](payload, length);
            break;
        }
        
    }

private:
    PubSubClient m_client; 
    const bool m_auto_reconnect;
    const char* m_name; 
    const uint16_t m_auto_reconnect_delay;
    
    const char* m_sub_map_topics[MSubs];
    SubscriberOnUpdate m_sub_map_callback[MSubs];
    MQTT_MSUBS_MAX_DTYPE m_sub_map_size = 0;
};

}; // namespace MQTT