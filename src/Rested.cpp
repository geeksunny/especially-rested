#include "Rested.h"

namespace rested {

////////////////////////////////////////////////////////////////
// Class : BaseClient //////////////////////////////////////////
////////////////////////////////////////////////////////////////

int BaseClient::getPort() {
  return port_;
}

void BaseClient::setPort(int port) {
  port_ = port;
}

////////////////////////////////////////////////////////////////
// Class : RestClient //////////////////////////////////////////
////////////////////////////////////////////////////////////////

RestClient::RestClient(const char *host, int port) {
  // todo: construct client_
}

WiFiClient *RestClient::getClient() {
  return &client_;
}

////////////////////////////////////////////////////////////////
// Class : RestClientSecure ////////////////////////////////////
////////////////////////////////////////////////////////////////

RestClientSecure::RestClientSecure(const char *host, int port, const char *fingerprint) {
  // todo: construct client_
}

WiFiClient *RestClientSecure::getClient() {
  return &client_;
}

////////////////////////////////////////////////////////////////
// Class : RestInterface ///////////////////////////////////////
////////////////////////////////////////////////////////////////

template<typename HttpClient>
void RestInterface<HttpClient>::addHeader(const char *header) {
  // TODO
}

template<typename HttpClient>
void RestInterface<HttpClient>::clearHeaders() {
  // TODO
}

template<typename HttpClient>
void RestInterface<HttpClient>::setClearHeadersAfterRequest(bool clear_after_request) {
  // TODO
}

////////////////////////////////////////////////////////////////
// Class : StringInterface /////////////////////////////////////
////////////////////////////////////////////////////////////////

template<typename HttpClient>
int StringInterface<HttpClient>::request(const char *method, const char *path, const char *body, String *response) {
  // TODO
  return 0;
}

template<typename HttpClient>
int StringInterface<HttpClient>::get(const char *path, String *response) {
  // TODO
  return 0;
}

template<typename HttpClient>
int StringInterface<HttpClient>::post(const char *path, const char *body, String *response) {
  // TODO
  return 0;
}

template<typename HttpClient>
int StringInterface<HttpClient>::patch(const char *path, const char *body, String *response) {
  // TODO
  return 0;
}

template<typename HttpClient>
int StringInterface<HttpClient>::put(const char *path, const char *body, String *response) {
  // TODO
  return 0;
}

template<typename HttpClient>
int StringInterface<HttpClient>::del(const char *path, String *response) {
  // TODO
  return 0;
}

template<typename HttpClient>
int StringInterface<HttpClient>::del(const char *path, const char *body, String *response) {
  // TODO
  return 0;
}

////////////////////////////////////////////////////////////////
// Class : StreamInterface /////////////////////////////////////
////////////////////////////////////////////////////////////////

template<typename HttpClient>
RestResponse<HttpClient> StreamInterface<HttpClient>::request(const char *method, const char *path, const char *body) {
  // TODO
  return RestResponse<HttpClient>(0, this);
}

template<typename HttpClient>
RestResponse<HttpClient> StreamInterface<HttpClient>::get(const char *path) {
  // TODO
  return RestResponse<HttpClient>(0, this);
}

template<typename HttpClient>
RestResponse<HttpClient> StreamInterface<HttpClient>::post(const char *path, const char *body) {
  // TODO
  return RestResponse<HttpClient>(0, this);
}

template<typename HttpClient>
RestResponse<HttpClient> StreamInterface<HttpClient>::patch(const char *path, const char *body) {
  // TODO
  return RestResponse<HttpClient>(0, this);
}

template<typename HttpClient>
RestResponse<HttpClient> StreamInterface<HttpClient>::put(const char *path, const char *body) {
  // TODO
  return RestResponse<HttpClient>(0, this);
}

template<typename HttpClient>
RestResponse<HttpClient> StreamInterface<HttpClient>::del(const char *path) {
  // TODO
  return RestResponse<HttpClient>(0, this);
}

template<typename HttpClient>
RestResponse<HttpClient> StreamInterface<HttpClient>::del(const char *path, const char *body) {
  // TODO
  return RestResponse<HttpClient>(0, this);
}

template<typename HttpClient>
void StreamInterface<HttpClient>::finish() {
  // TODO
}

////////////////////////////////////////////////////////////////
// Class : RestResponse ////////////////////////////////////////
////////////////////////////////////////////////////////////////
template<typename HttpClient>
RestResponse<HttpClient>::RestResponse(int status_code, StreamInterface<HttpClient> *client) : statusCode_(status_code), client_(client) {

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
size_t RestResponse<HttpClient>::write(uint8_t uint_8) {
  // This method does nothing. Do not use it.
  // TODO: Add a static_assert or some other method to throw compiler warning/error when this is called somewhere.
  return 0;
}

template<typename HttpClient>
int RestResponse<HttpClient>::available() {
  return (client_ == nullptr) ? false : client_->getClient()->available();
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
void RestResponse<HttpClient>::finish() {
  // TODO: check if already finished
  // TODO: call client_.finish()
  // TODO: Clean up if necessary
}

template class StringInterface<RestClient>;
template class StringInterface<RestClientSecure>;
template class StreamInterface<RestClient>;
template class StreamInterface<RestClientSecure>;
template class RestResponse<RestClient>;
template class RestResponse<RestClientSecure>;

}
