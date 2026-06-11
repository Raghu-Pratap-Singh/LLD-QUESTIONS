#include <bits/stdc++.h>
using namespace std;

// we will use uploaderand downloader seperately as high level modules should never interact with low level modules : (dependency inversion principle)

class File {
public:
    virtual ~File() = default;
};

class Text_File : public File {
private:
    string content;
public:
    Text_File (string text) {
        this->content = text;
    }
};

class Image_File : public File {
private:
    string path;
public:
    Image_File (string path) {
        this->path = path;
    }
};


class Uploader {
public:
    virtual void upload_file (File* f) = 0;
    virtual ~Uploader() = default;
};

class Downloader {
public:
    virtual void download_file (File* f) = 0;
    virtual ~Downloader() = default;

};


class AWS_uploader : public Uploader {
public:
    void upload_file(File* f) {
        cout << "aws is uploading file..." << endl;
    }
};

class AWS_downloader : public Downloader {
public:
    void download_file(File* f) {
        cout << "aws is downloading file..." << endl;
    }
};

class Google_uploader : public Uploader {
public:
    void upload_file(File* f) {
        cout << "Google is uploading file..." << endl;
    }
};

class Google_downloader : public Downloader {
public:
    void download_file(File* f) {
        cout << "Google is downloading file..." << endl;
    }
};

class Azure_uploader : public Uploader {
public:
    void upload_file(File* f) {
        cout << "Azure is uploading file..." << endl;
    }
};

class Azure_downloader : public Downloader {
public:
    void download_file(File* f) {
        cout << "Azure is downloading file..." << endl;
    }
};

// abstract
class Provider {
protected:
    Uploader* u;
    Downloader* d;
public:
    Provider () {};
    virtual void upload(File* f) = 0;
    virtual void download(File* f) = 0;
    virtual ~Provider() {
        delete u;
        delete d;
    }
};

// concrete
class AWS_S3 : public Provider {
public:
    AWS_S3() {
        this->u = new AWS_uploader();
        this->d = new AWS_downloader();
    };
    void upload (File* f) override {
        u->upload_file(f);
    }
    void download (File* f) override {
        d->download_file(f);
    }

};
class Google_Cloud_Storage : public Provider {
public:
    Google_Cloud_Storage () {
        this->u = new Google_uploader();
        this->d = new Google_downloader();
    };
    void upload (File* f) override {
        u->upload_file(f);
    }
    void download (File* f) override {
        d->download_file(f);
    }
};
class Azure_Blob_Storage : public Provider {
public:
    Azure_Blob_Storage () {
        this->u = new Azure_uploader();
        this->d = new Azure_downloader();
    };
    void upload (File* f) override {
        u->upload_file(f);
    }
    void download (File* f) override {
        d->download_file(f);
    }
};


class Provider_Factory {
public:
    Provider_Factory () {};
    Provider* create_provider(string& type) {
        if (type == "AWS S3") {
            cout << "creating AWS_S3" << endl;
            return new AWS_S3();
        } else if (type == "Google Cloud Storage") {
            cout << "creating Google Cloud Storage" << endl;
            return new Google_Cloud_Storage();
        } else if (type == "Azure Blob Storage") {
            cout << "creating Azure Blob Storage" << endl;
            return new Azure_Blob_Storage();
        } else {
            cout << "invalid provider type.." << endl;
            return nullptr;
        }
    }

    ~Provider_Factory() = default;
};




class Storage_Application {
private:
    Provider* provider;
    static Storage_Application* instance;

    Storage_Application () {
        this->provider = nullptr;
    };
    
public:
    static Storage_Application* get_instance() {
        
        
        return instance;
    }
    void set_provider(Provider* p) {
        instance->provider = p;
    }
    void upload_file(File* f) {
        instance->provider->upload(f);
    }

    void download_file(File* f) {
        instance->provider->download(f);
    }

    ~Storage_Application () {
        delete this->provider;
    }
};

// we will use eager initialisation
Storage_Application* Storage_Application::instance = new Storage_Application();


int main () {
    // we will seperate provider creation from user
    Provider_Factory* factory = new Provider_Factory();
    string type = "Google Cloud Storage";
    // user will create any type of provider he wants
    Provider* user_provider = factory->create_provider(type);

    Storage_Application* s1 = Storage_Application::get_instance();
    s1->set_provider(user_provider);

    // now user can mention file to be downloaded or uploaded here
    File* f = new Text_File("python_docs here, this file contains data about python project");

    // upload or download
    s1->upload_file(f);

    return 0;
}