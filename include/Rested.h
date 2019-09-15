#ifndef ESPECIALLY_RESTED_INCLUDE_RESTED_H_
#define ESPECIALLY_RESTED_INCLUDE_RESTED_H_

#include <WiFiClientSecure.h>

#define REST_RESPONSE_ERROR           -1
#define REST_RESPONSE_STOPPED          0

namespace rested {

template<typename HttpClient> class RestResponse;

class BaseClient {
 public:
  int getPort();
  void setPort(int port);
 protected:
  virtual WiFiClient *getClient() = 0;
 private:
  const char *host_;
  int port_;
};

class RestClient : public BaseClient {
 public:
  explicit RestClient(const char *host, int port = 80);
 protected:
  WiFiClient *getClient() override;
 private:
  WiFiClient client_;
};

class RestClientSecure : public BaseClient {
 public:
  explicit RestClientSecure(const char *host, int port = 443, const char* fingerprint = "");
  // TODO: fingerprint setter(/getter?)
 protected:
  WiFiClient *getClient() override;
 private:
  WiFiClientSecure client_;
};

template<typename HttpClient>
class RestInterface : public HttpClient {
  friend class RestResponse<HttpClient>;
  // TODO: virtual methods for readResponse, cleanup
 public:
  using HttpClient::HttpClient;
  void addHeader(const char *header);
  void clearHeaders();
  void setClearHeadersAfterRequest(bool clear_after_request);
};

template<typename HttpClient>
class StringInterface : public RestInterface<HttpClient> {
 public:
  using RestInterface<HttpClient>::RestInterface;
  int request(const char *method, const char *path, const char *body, String *response = nullptr);
  int get(const char *path, String *response = nullptr);
  int post(const char *path, const char *body, String *response = nullptr);
  int patch(const char *path, const char *body, String *response = nullptr);
  int put(const char *path, const char *body, String *response = nullptr);
  int del(const char *path, String *response = nullptr);
  int del(const char *path, const char *body, String *response = nullptr);
};

template<typename HttpClient>
class StreamInterface : public RestInterface<HttpClient> {
 public:
  using RestInterface<HttpClient>::RestInterface;
  RestResponse<HttpClient> request(const char *method, const char *path, const char *body);
  RestResponse<HttpClient> get(const char *path);
  RestResponse<HttpClient> post(const char *path, const char *body);
  RestResponse<HttpClient> patch(const char *path, const char *body);
  RestResponse<HttpClient> put(const char *path, const char *body);
  RestResponse<HttpClient> del(const char *path);
  RestResponse<HttpClient> del(const char *path, const char *body);
  void finish();
};

template<typename HttpClient>
class RestResponse : public Stream {
  // TODO: friend class
  // TODO: could the template be removed here somehow?
  // TODO: add fields/accessors for content-type / response headers ?

 public:
  explicit RestResponse(int status_code, StreamInterface<HttpClient> *client);

  explicit operator bool();
  explicit operator bool() const;

  size_t write(uint8_t uint_8) override;
  int available() override;
  int read() override;
  int peek() override;
  void finish();

 private:
  int statusCode_ = 0;
  StreamInterface<HttpClient> *client_;
};

typedef StringInterface<RestClient> BasicRestClient;
typedef StringInterface<RestClientSecure> SecureRestClient;
typedef StreamInterface<RestClient> StreamedBasicRestClient;
typedef StreamInterface<RestClientSecure> StreamedSecureRestClient;

}

#endif //ESPECIALLY_RESTED_INCLUDE_RESTED_H_
