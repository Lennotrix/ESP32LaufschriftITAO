#define HTTP_MAX_LEN 4096

class http{
  public:
    http(const char* uName, const char* pw);
    ~http();
    char* GetPhrase(char* endpoint);
    char* Login();
    int GetStatus();

    private:
      int status;
      const char* uName;
      const char* uPw;
      char* pBearer;
};