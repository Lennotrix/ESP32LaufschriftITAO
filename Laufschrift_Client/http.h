#define HTTP_MAX_LEN 512

class http{
  public:
    http(char* endpoint);
    http(char* endpoint, char* token);
    http(char* endpoint, char* testJSON, int bum);
    ~http();
    char* GetPhrase();
    int GetStatus();

    private:
      char* Get();
      int status;
      const char* endpoint;
      const char* token;
      char* testJSON;
};