class GlobRewrite : public AsyncWebRewrite {
  protected:
    String _urlPrefix;
    int _globIndex;
    String _paramsBackup;

  public:
  GlobRewrite(const char* from, const char* to) : AsyncWebRewrite(from, to) {

      _globIndex = _from.indexOf('*');

      if( _globIndex >=0 ) {
        _urlPrefix = _from.substring(0, _globIndex);
        int index = _params.indexOf('*');
        _params = _params.substring(0,index);
      }
      
      else _urlPrefix = _from;
      
      _paramsBackup = _params;
  }

  bool match(AsyncWebServerRequest *request) override {
    if(request->url().startsWith(_urlPrefix)) {
      if(_globIndex >= 0) _params = _paramsBackup + request->url().substring(_globIndex);
      else _params = _paramsBackup;
      return true;
    }
    else return false;
  }
};