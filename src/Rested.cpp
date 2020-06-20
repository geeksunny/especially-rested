#include "Rested.h"

#ifndef RESTED_READ_DELAY
#define RESTED_READ_DELAY 50
#endif

namespace rested {

////////////////////////////////////////////////////////////////
// Class : BaseClient //////////////////////////////////////////
////////////////////////////////////////////////////////////////

BaseClient::BaseClient(const char *host, uint16_t port, const char *content_type) : host_(host), port_(port) {
  contentType_ = (content_type == nullptr) ? "application/x-www-form-urlencoded" : content_type;
}

bool BaseClient::connected() {
  return getClient()->connected();
}

uint16_t BaseClient::getPort() {
  return port_;
}

void BaseClient::setPort(uint16_t port) {
  port_ = port;
}

String BaseClient::getContentType() {
  return contentType_;
}

void BaseClient::setContentType(const char *content_type) {
  contentType_ = content_type;
}

BaseClient::operator bool() {
  return host_ != nullptr;
}

BaseClient::operator bool() const {
  return host_ != nullptr;
}

////////////////////////////////////////////////////////////////
// Class : RestClient //////////////////////////////////////////
////////////////////////////////////////////////////////////////

RestClient::RestClient(const char *host, uint16_t port, const char *content_type) : BaseClient(host, port, content_type) {
  //
}

WiFiClient *RestClient::getClient() {
  return &client_;
}

////////////////////////////////////////////////////////////////
// Class : RestClientSecure ////////////////////////////////////
////////////////////////////////////////////////////////////////

RestClientSecure::RestClientSecure(const char *host, uint16_t port, const char *fingerprint, const char *content_type)
    : BaseClient(host, port, content_type) {
  setFingerprint(fingerprint);
}

RestClientSecure::RestClientSecure(const bool require_self_signed_cert,
                                   const char *host,
                                   uint16_t port,
                                   const char *content_type) : BaseClient(host, port, content_type) {
  setRequireSelfSignedCert(require_self_signed_cert);
}

WiFiClient *RestClientSecure::getClient() {
  return &client_;
}

void RestClientSecure::setFingerprint(const char *fingerprint) {
  if (fingerprint) {
    client_.setFingerprint(fingerprint);
  } else {
    client_.setInsecure();
  }
}

void RestClientSecure::setRequireSelfSignedCert(bool require_self_signed_cert) {
  if (require_self_signed_cert) {
    client_.allowSelfSignedCerts();
  } else {
    client_.setInsecure();
  }
}

////////////////////////////////////////////////////////////////
// Class : RestInterface ///////////////////////////////////////
////////////////////////////////////////////////////////////////

template<typename HttpClient>
bool RestInterface<HttpClient>::addHeader(const char *header) {
  if (headerCount_ < REST_HEADER_MAX) {
    headers_[headerCount_++] = header;
    return true;
  }
  return false;
}

template<typename HttpClient>
void RestInterface<HttpClient>::clearHeaders() {
  headerCount_ = 0;
}

template<typename HttpClient>
void RestInterface<HttpClient>::setClearHeadersAfterRequest(bool clear_after_request) {
  clearHeadersAfterRequest_ = clear_after_request;
}

template<typename HttpClient>
bool RestInterface<HttpClient>::makeRequest(const char *method, const char *path, const char *body) {
  if (started_) {
    finish();
  }
  WiFiClient *client = this->getClient();
  if (client->connect(HttpClient::host_, HttpClient::port_)) {
    started_ = true;
    client->println(String(method) + " " + String(path) + " HTTP/1.1");
    for (uint8_t i = 0; i < headerCount_; ++i) {
      client->println(String(headers_[i]));
    }
    client->println("Host: " + String(HttpClient::host_) + ":" + String(HttpClient::port_));
    client->println("Connection: close");
    if (body) {
      client->println("Content-Length: " + String(strlen(body)));
      client->print("Content-Type: " + String(HttpClient::contentType_));
      client->print("\r\n\r\n");
      client->println(body);
      client->print("\r\n\r\n");
    } else {
      client->print("\r\n");
    }
    return true;
  }
  return false;
}

template<typename HttpClient>
uint16_t RestInterface<HttpClient>::readResponse() {
  WiFiClient *client = HttpClient::getClient();
  delay(RESTED_READ_DELAY);
  if (client->available() && client->findUntil(" ", "\r\n")) {
    uint16_t httpStatus = client->parseInt();
    if (httpStatus != 0) {
      // TODO: Read the rest of this line for HTTP status reason phrase
      // TODO: Parse each line of the response, allow debug printing, save some(/all) values of the response?
      // Fast tracking to the response content.
      client->find("\r\n\r\n");
      return httpStatus;
    }
  }
  // Error result
  return REST_RESPONSE_ERROR;
}

template<typename HttpClient>
bool RestInterface<HttpClient>::isStarted() {
  return started_;
}

template<typename HttpClient>
void RestInterface<HttpClient>::finish() {
  if (!started_) {
    return;
  }
  // Cleanup
  if (clearHeadersAfterRequest_) {
    headerCount_ = 0;
  }
  HttpClient::getClient()->stop();
  started_ = false;
  delay(RESTED_READ_DELAY);  // Necessary?
}

////////////////////////////////////////////////////////////////
// Class : StringInterface /////////////////////////////////////
////////////////////////////////////////////////////////////////

template<typename HttpClient>
uint16_t StringInterface<HttpClient>::request(const char *method, const char *path, const char *body, String *response) {
  if (this->makeRequest(method, path, body)) {
    uint16_t statusCode = this->readResponse();
    if (response != nullptr) {
      WiFiClient *client = this->getClient();
      response->reserve(response->length() + client->available());
      while (client->available()) {
        *response += (char) client->read();
      }
    }
    this->finish();
    return statusCode;
  }
  return 0;
}

template<typename HttpClient>
uint16_t StringInterface<HttpClient>::get(const char *path, String *response) {
  return request("GET", path, nullptr, response);
}

template<typename HttpClient>
uint16_t StringInterface<HttpClient>::post(const char *path, const char *body, String *response) {
  return request("POST", path, body, response);
}

template<typename HttpClient>
uint16_t StringInterface<HttpClient>::patch(const char *path, const char *body, String *response) {
  return request("PATCH", path, body, response);
}

template<typename HttpClient>
uint16_t StringInterface<HttpClient>::put(const char *path, const char *body, String *response) {
  return request("PUT", path, body, response);
}

template<typename HttpClient>
uint16_t StringInterface<HttpClient>::del(const char *path, String *response) {
  return request("DEL", path, nullptr, response);
}

template<typename HttpClient>
uint16_t StringInterface<HttpClient>::del(const char *path, const char *body, String *response) {
  return request("DEL", path, body, response);
}

////////////////////////////////////////////////////////////////
// Class : StreamInterface /////////////////////////////////////
////////////////////////////////////////////////////////////////

template<typename HttpClient>
RestResponse<HttpClient> StreamInterface<HttpClient>::request(const char *method, const char *path, const char *body) {
  if (this->makeRequest(method, path, body)) {
    uint16_t statusCode = this->readResponse();
    return RestResponse<HttpClient>(statusCode, this);
  }
  return RestResponse<HttpClient>(0, this);
}

template<typename HttpClient>
RestResponse<HttpClient> StreamInterface<HttpClient>::get(const char *path) {
  return request("GET", path, nullptr);
}

template<typename HttpClient>
RestResponse<HttpClient> StreamInterface<HttpClient>::post(const char *path, const char *body) {
  return request("POST", path, body);
}

template<typename HttpClient>
RestResponse<HttpClient> StreamInterface<HttpClient>::patch(const char *path, const char *body) {
  return request("PATCH", path, body);
}

template<typename HttpClient>
RestResponse<HttpClient> StreamInterface<HttpClient>::put(const char *path, const char *body) {
  return request("PUT", path, body);
}

template<typename HttpClient>
RestResponse<HttpClient> StreamInterface<HttpClient>::del(const char *path) {
  return request("DEL", path, nullptr);
}

template<typename HttpClient>
RestResponse<HttpClient> StreamInterface<HttpClient>::del(const char *path, const char *body) {
  return request("DEL", path, body);
}

////////////////////////////////////////////////////////////////
// Class : RestResponse ////////////////////////////////////////
////////////////////////////////////////////////////////////////

template<typename HttpClient>
RestResponse<HttpClient>::RestResponse(uint16_t status_code, StreamInterface<HttpClient> *client)
    : statusCode_(status_code), client_(client) {

}

template<typename HttpClient>
RestResponse<HttpClient>::operator bool() {
  return statusCode_ > REST_RESPONSE_STOPPED;
}

template<typename HttpClient>
RestResponse<HttpClient>::operator bool() const {
  return statusCode_ > REST_RESPONSE_STOPPED;
}

template<typename HttpClient>
__attribute__((deprecated)) size_t RestResponse<HttpClient>::write(uint8_t uint_8) {
  // This method does nothing. Do not use it.
  return 0;
}

template<typename HttpClient>
int RestResponse<HttpClient>::available() {
  if (client_ == nullptr) {
    return false;
  }
  WiFiClient *client = client_->getClient();
  // BLOCK the thread if available() is zero but connected() is true.
  // This will return once one of these conditions is not true.
  while (!client->available() && client->connected()) {
    yield();
  }
  return client->available();
}

template<typename HttpClient>
int RestResponse<HttpClient>::read() {
  return (client_ == nullptr) ? -1 : client_->getClient()->read();
}

template<typename HttpClient>
int RestResponse<HttpClient>::peek() {
  return (client_ == nullptr) ? -1 : client_->getClient()->peek();
}

template<typename HttpClient>
bool RestResponse<HttpClient>::connected() {
  return (client_ == nullptr) ? false : client_->getClient()->connected();
}

template<typename HttpClient>
int RestResponse<HttpClient>::statusCode() {
  return statusCode_;
}

template<typename HttpClient>
void RestResponse<HttpClient>::finish() {
  if (client_) {
    client_->finish();
  }
}

template class StringInterface<RestClient>;
template class StringInterface<RestClientSecure>;
template class StreamInterface<RestClient>;
template class StreamInterface<RestClientSecure>;
template class RestResponse<RestClient>;
template class RestResponse<RestClientSecure>;

}
