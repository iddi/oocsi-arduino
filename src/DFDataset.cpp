/***************************************************************************
 * The DataFoundry library for the ESP32 and ESP 8266 to store and retrieve
 * data from ESPs on the Data Foundry platform.
 *
 * Developed by Mathias Funk
 **************************************************************************/

#include "DFDataset.h"

/**
 * @brief  Constructor for creating a new DFDataset
 * @note   Should only be called once
 * @retval A new DFDataset client
 */
DFDataset::DFDataset(const char* server, int id, const char* token) {
  host = server;
  dataset_id = id;
  api_token = token;
  logging = true;
  jsonMessage.clear();
}

// iot datasets
void DFDataset::device(const char* device) {
  device_id = device;
}

void DFDataset::activity(const char* act) {
  activity_id = act;
}

// entity datasets
void DFDataset::id(const char* id) {
  resource_id = id;
}

void DFDataset::token(const char* token) {
  resource_token = token;
}

const char* root_ca_df = "-----BEGIN CERTIFICATE-----\n" \
                      "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \
                      "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
                      "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \
                      "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
                      "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
                      "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
                      "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
                      "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
                      "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
                      "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
                      "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
                      "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
                      "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
                      "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
                      "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
                      "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
                      "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
                      "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
                      "-----END CERTIFICATE-----";

bool DFDataset::logItem() {

  // check basic input data
  if(host == NULL || api_token == NULL || dataset_id == 0) {
    return false;
  }

  // check message
  String postMessage = String();
  serializeJson(jsonMessage, postMessage);
  if(postMessage == NULL || postMessage == "null") {
    return false;
  }

  // set missing data
  activity_id = activity_id != NULL ? activity_id : "";
  device_id = device_id != NULL ? device_id : "";

  // compile address
  snprintf_P(address, sizeof(address), PSTR("http://%s/datasets/ts/log/%i/%s"), host, dataset_id, activity_id);

  // do transmission
  HTTPClient http;
  http.begin(address/*, root_ca_df*/);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("api_token", api_token);
  http.addHeader("source_id", device_id);
  http.addHeader("device_id", device_id);

  int httpCode = http.POST(postMessage);
  jsonMessage.clear();

  http.end();

  return httpCode == HTTP_CODE_OK;
}

bool DFDataset::addItem() {

  // check basic input data
  if(host == NULL || api_token == NULL || resource_id == NULL || dataset_id == 0) {
    return false;
  }

  // check message
  String postMessage = String();
  serializeJson(jsonMessage, postMessage);

  // compile address
  snprintf_P(address, sizeof(address), PSTR("http://%s/datasets/entity/%i/item/"), host, dataset_id);

  // set missing data
  resource_token = resource_token != NULL ? resource_token : "";

  // do transmission
  HTTPClient http;
  http.begin(address/*, root_ca_df*/);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("resource_id", resource_id);
  http.addHeader("token", resource_token);
  http.addHeader("api_token", api_token);

  int httpCode = http.POST(postMessage);
  jsonMessage.clear();

  http.end();

  return httpCode == HTTP_CODE_OK;
}

bool DFDataset::updateItem() {

  // check basic input data
  if(host == NULL || api_token == NULL || resource_id == NULL || dataset_id == 0) {
    return false;
  }

  // check message
  String postMessage = String();
  serializeJson(jsonMessage, postMessage);

  // compile address
  snprintf_P(address, sizeof(address), PSTR("http://%s/datasets/entity/%i/item/"), host, dataset_id);

  // set missing data
  resource_token = resource_token != NULL ? resource_token : "";

  // do transmission
  HTTPClient http;
  http.begin(address/*, root_ca_df*/);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("resource_id", resource_id);
  http.addHeader("token", resource_token);
  http.addHeader("api_token", api_token);

  int httpCode = http.PUT(postMessage);
  jsonMessage.clear();

  http.end();

  return httpCode == HTTP_CODE_OK;
}

bool DFDataset::deleteItem() {
  
  // check basic input data
  if(host == NULL || api_token == NULL || resource_id == NULL || dataset_id == 0) {
    return false;
  }

  // compile address
  snprintf_P(address, sizeof(address), PSTR("http://%s/datasets/entity/%i/item/"), host, dataset_id);
  
  // set missing data
  resource_token = resource_token != NULL ? resource_token : "";

  // do transmission
  HTTPClient http;
  http.begin(address/*, root_ca_df*/);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("resource_id", resource_id);
  http.addHeader("token", resource_token);
  http.addHeader("api_token", api_token);
  
  int httpCode = http.sendRequest("DELETE");
  jsonMessage.clear();

  String jsonResponse = http.getString();

  http.end();

  return httpCode == HTTP_CODE_OK;
}

bool DFDataset::getItem() {
  // check basic input data
  if(host == NULL || api_token == NULL || resource_id == NULL || dataset_id == 0) {
    return false;
  }

  // compile address
  snprintf_P(address, sizeof(address), PSTR("http://%s/datasets/entity/%i/item/"), host, dataset_id);

  // set missing data
  resource_token = resource_token != NULL ? resource_token : "";

  // do transmission
  HTTPClient http;
  http.begin(address/*, root_ca_df*/);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("resource_id", resource_id);
  http.addHeader("token", resource_token);
  http.addHeader("api_token", api_token);

  int httpCode = http.GET();
  jsonMessage.clear();

  String jsonResponse = http.getString();

  http.end();

  if(jsonResponse.length() > 0) {
    DeserializationError error = deserializeJson(jsonDocument, jsonResponse.c_str());
    if (error) {
      print(F("Message parsing failed: "));
      println(error.c_str());
      return false;
    }
    return true;
  } else {
    return false;
  }
}

bool DFDataset::getBool(const char* key, bool standard) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    return standard;
  } else {
    return jsonDocument[key].as<bool>();
  }
}

int DFDataset::getInt(const char* key, int standard) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    return standard;
  } else {
    return jsonDocument[key].as<int>();
  }
}

long DFDataset::getLong(const char* key, long standard) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    return standard;
  } else {
    return jsonDocument[key].as<long>();
  }
}

float DFDataset::getFloat(const char* key, float standard) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    return standard;
  } else {
    return jsonDocument[key].as<float>();
  }
}

String DFDataset::getString(const char* key, const char* standard) {
  if(jsonDocument.isNull() || !jsonDocument.containsKey(key)) {
    return standard;
  } else {
    return jsonDocument[key].as<String>();
  }
}

bool DFDataset::has(const char* key) {
  return jsonDocument.containsKey(key);
}

// function for outputting all (top-level) keys in the message as a comma-separated list
String DFDataset::keys() {
  if(jsonDocument.isNull() || jsonDocument.size() == 0) {
    return "";
  }

  // comma separated key list
  String result = "";
  for (JsonPair p : jsonDocument.as<JsonObject>()) {
      if(result.length() > 0) {
        result.concat(",");  
      }
      result.concat(p.key().c_str());
  }
  return result;
}

// function for sending a bool to DFDataset
DFDataset DFDataset::addBool(const char* key, bool value) {
  jsonMessage[key] = value;
  return *this;
}

// function for sending an int to DFDataset
DFDataset DFDataset::addInt(const char* key, int value) {
  jsonMessage[key] = value;
  return *this;
}

// function for sending an int to DFDataset
DFDataset DFDataset::addLong(const char* key, long value) {
  jsonMessage[key] = value;
  return *this;
}

// function for sending a float to DFDataset
DFDataset DFDataset::addFloat(const char* key, float value) {
  jsonMessage[key] = value;
  return *this;
}

DFDataset DFDataset::addString(const char* key, const char* value) {
  jsonMessage[key] = value;
  return *this;
}

void DFDataset::print(const String &message) {
  if(logging)
    Serial.print(message);
}

void DFDataset::print(char message) {
  if(logging)
    Serial.print(message);
}

void DFDataset::println() {
  if(logging)
    Serial.println();
}

void DFDataset::println(const String &message) {
  if(logging)
    Serial.println(message);
}

void DFDataset::println(char message) {
  if(logging)
    Serial.println(message);
}

void DFDataset::setActivityLEDPin(int ledPin) {
  activityLEDPin = ledPin;
}

void DFDataset::setLogging(bool log) {
  logging = log;
}
