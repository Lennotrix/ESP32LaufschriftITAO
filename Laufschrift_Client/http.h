#define HTTP_MAX_LEN 4096
#include <Arduino_JSON.h>
#include <HTTPClient.h>

class http{
  public:
    http(const char* uName, const char* pw);
    ~http();
    char* GetPhrase();
    char* Login();

    private:
      void appendPhrase(JSONVar &jsonObject, char *phrase);
      const char* uName;
      const char* uPw;
      char* pBearer;
};