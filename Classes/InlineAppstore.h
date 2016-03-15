//
//  InlineAppstore.h
//  FusePuzzle
//
//  Created by 梁志远 on 11/27/14.
//
//

class CCAppStore
{
private:
    static CCAppStore * _appStore;
public:
    
    static CCAppStore * sharedInstance()
    {
        if (!_appStore) {
            _appStore = new CCAppStore();
        }
        return _appStore;
    }

    void ShowInAppstore(char * app_id);
};
