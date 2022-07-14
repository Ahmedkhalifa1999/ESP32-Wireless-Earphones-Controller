#include <stdio.h>

#include "nvs.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"

#include "esp_log.h"


void bt_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    ESP_LOGI("BT", "Event: %d", event);
    switch (event)
    {
        case ESP_BT_GAP_DISC_RES_EVT:
            esp_bt_gap_read_remote_name(param->disc_res.bda);
            break;
            
        case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
             if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED)
            {
                ESP_LOGI("BT", "Discovery Restarted");
                esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 5, 0);
            }
            break;

        case ESP_BT_GAP_READ_REMOTE_NAME_EVT:
            ESP_LOGI("BT", "Discovered %s", param->read_rmt_name.rmt_name);
            break;

        default:
            break;
    }
}


void app_main(void)
{
     esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if (esp_bt_controller_init(&bt_cfg) != ESP_OK) {
        ESP_LOGE("BT", "%s initialize controller failed\n", __func__);
        return;
    }
    if (esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT) != ESP_OK) {
        ESP_LOGE("BT", "%s enable controller failed\n", __func__);
        return;
    }
    if (esp_bluedroid_init() != ESP_OK) {
        ESP_LOGE("BT", "%s initialize bluedroid failed\n", __func__);
        return;
    }
    if (esp_bluedroid_enable() != ESP_OK) {
        ESP_LOGE("BT", "%s enable bluedroid failed\n", __func__);
        return;
    }
    esp_bt_dev_set_device_name("My ESP32");
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_LIMITED_DISCOVERABLE);

    esp_bt_gap_register_callback(bt_callback);
    esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_LIMITED_INQUIRY, 0x10, 0);
}
