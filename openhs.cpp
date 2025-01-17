#include "openhs.h"
#include <BLEDevice.h>

/** Random device address */
esp_bd_addr_t rnd_addr = {0xFF, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

#include "key.h"

/** Advertisement payload */
uint8_t adv_data[31] = {
    0x1e, /* Length (30) */
    0xff, /* Manufacturer Specific Data (type 0xff) */
    0x4c, 0x00, /* Company ID (Apple) */
    0x12, 0x19, /* Offline Finding type and length */
    0x00, /* State */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, /* First two bits */
    0x00, /* Hint (0x00) */
};

/* https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html#_CPPv420esp_ble_adv_params_t */
static esp_ble_adv_params_t ble_adv_params = {
    // Advertising min interval:
    // Minimum advertising interval for undirected and low duty cycle
    // directed advertising. Range: 0x0020 to 0x4000 Default: N = 0x0800
    // (1.28 second) Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec
    .adv_int_min = 0x0640, // 1s
    // Advertising max interval:
    // Maximum advertising interval for undirected and low duty cycle
    // directed advertising. Range: 0x0020 to 0x4000 Default: N = 0x0800
    // (1.28 second) Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec
    .adv_int_max = 0x0C80, // 2s
    // Advertisement type
    .adv_type = ADV_TYPE_NONCONN_IND,
    // Use the random address
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
    .peer_addr = {
        0x00,
    },
    .peer_addr_type = BLE_ADDR_TYPE_PUBLIC,

    // All channels
    .channel_map = ADV_CHNL_ALL,
    // Allow both scan and connection requests from anyone.
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
  esp_err_t err;

  switch (event) {
  case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
    esp_ble_gap_start_advertising(&ble_adv_params);
    break;

  case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
    //adv start complete event to indicate adv start successfully or failed
    if ((err = param->adv_start_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
      Serial.printf("advertising start failed: %s\n", esp_err_to_name(err));
    } else {
      Serial.printf("advertising has started.\n");
    }
    break;

  case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
    if ((err = param->adv_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
      Serial.printf("adv stop failed: %s\n", esp_err_to_name(err));
    } else {
      Serial.printf("stop adv successfully\n");
    }
    break;
  default:
    break;
  }
}

void set_addr_from_key(esp_bd_addr_t addr, uint8_t *public_key) {
  addr[0] = public_key[0] | 0b11000000;
  addr[1] = public_key[1];
  addr[2] = public_key[2];
  addr[3] = public_key[3];
  addr[4] = public_key[4];
  addr[5] = public_key[5];
}

void set_payload_from_key(uint8_t *payload, uint8_t *public_key) {
  /* copy last 22 bytes */
  memcpy(&payload[7], &public_key[6], 22);
  /* append two bits of public key */
  payload[29] = public_key[0] >> 6;
}

void OpenHSSetup() {
  set_addr_from_key(rnd_addr, public_key);
  set_payload_from_key(adv_data, public_key);

  BLEDevice::init("ACME");
  BLEDevice::setCustomGapHandler(esp_gap_cb);

  BLEAdvertisementData advertisementData = BLEAdvertisementData();
  advertisementData.setManufacturerData(std::string((char *)adv_data + 2, sizeof(adv_data) - 2));
 
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setScanResponse(true);
  pAdvertising->setMaxInterval(0x0C80);
  pAdvertising->setMinInterval(0x0640);
  pAdvertising->setDeviceAddress(rnd_addr);
  pAdvertising->setAdvertisementData(advertisementData);

  BLEDevice::startAdvertising();
}
