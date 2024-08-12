#include <stdexcept>
#include <iostream>
#include <vector>
#include "glib.h"
#include "src/backends/virtual.hpp"
#include <libnm/NetworkManager.h>

class NmBackend : public Backend {
    NMClient * client = NULL;
    GError *error = NULL;
    NMDeviceWifiP2P *p2p_device = NULL;
    private:
        static void discoveryCallback(GObject *source, GAsyncResult *res, gpointer user_data) {
            g_autoptr(GError) error = NULL;
            NMDeviceWifiP2P *p2p_device = NM_DEVICE_WIFI_P2P(source);
            if (!nm_device_wifi_p2p_start_find_finish(p2p_device, res, &error)) {
                throw std::runtime_error(std::string("P2P Discovery failure: ") + error->message);
            }
        }
    public:
        void beginDiscovery() {
            if (p2p_device == NULL) {
                throw std::runtime_error("P2P device not initialized!");
            }
            std::cout<< "Beginning P2P Discovery." << std::endl;
            nm_device_wifi_p2p_start_find(p2p_device, NULL, NULL, discoveryCallback, NULL);
        }

        NmBackend() {
            client = nm_client_new(NULL, &error);
            if (error != NULL) {
                throw std::runtime_error(error->message);
            } else if (client) {
                std::cout << "NM Client created, version " << nm_client_get_version(client) << std::endl;
            }
            const GPtrArray* devices = nm_client_get_devices(client);
            if (devices == NULL) {
                throw std::runtime_error("No devices found.");
            }
            for (int i = 0; i < (int) devices->len; i++) {
                NMDevice *device = NM_DEVICE(g_ptr_array_index(devices,i));
                if (nm_device_get_device_type(device) == NM_DEVICE_TYPE_WIFI_P2P){
                    p2p_device = NM_DEVICE_WIFI_P2P(device);
                    break;
                }
            }

        }
        std::vector<std::string> GetPeers() {
            std::vector<std::string> retlist = {};
            const GPtrArray *list = nm_device_wifi_p2p_get_peers(p2p_device);
            for (int i = 0; i < (int) list->len; i++) {
                NMWifiP2PPeer *peer = NM_WIFI_P2P_PEER(g_ptr_array_index(list, i));
                retlist.push_back(nm_wifi_p2p_peer_get_name(peer));
            }
            return retlist;
        }
};
