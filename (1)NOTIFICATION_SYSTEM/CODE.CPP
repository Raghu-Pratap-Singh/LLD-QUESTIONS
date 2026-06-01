#include <bits/stdc++.h>
using namespace std;

class User {
public:
    string name;
    string mobile;
protected:
    string email;
public:
    User (string name, string email, string mobile) {
        this->name = name;
        this->email = email;
        this->mobile = mobile;

    }
    // getter for email
    string getEmail() {
        return this->email;
    }
};
class NotificationType {
protected:
    string text;
public:
    NotificationType (string text) {
        this->text = text;
    }
};

class PromotioanlNotification : public NotificationType {
public:
    PromotionalNotification (string text) : NotificationType(text) {};
};

class SecurityNotification : public NotificationType {
public:
    SecurityNotification (string text) : NotificationType(text) {};
};

class TransactionNotification : public NotificationType {
public:
    TransactionNotification (string text) : NotificationType(text) {};
};

class notificationService {
public:
    virtual void sendNotification(User& user, NotificationType* message) = 0;
};


class WhatsappSender : public notificationService {
public:
    void sendNotification (User& user, NotificationType* message) {
        cout << "whatsapp sent" <<endl;
    }
};

class EmailSender : public notificationService {
public:
    void sendNotification (User& user, NotificationType* message) {
        cout << "Email sent" <<endl;
    }
};

class SMSSender : public notificationService {
public:
    void sendNotification (User& user, NotificationType* message) {
        cout << "SMS sent" <<endl;
    }
};

class SlackSender : public notificationService {
public:
    void sendNotification (User& user, NotificationType* message) {
        cout << "Slack sent" <<endl;
    }
};

class HighprioritySender : public notificationService {
public:
    void sendNotification (User& user, NotificationType* message) {
        cout << "sent to all" <<endl;
    }
};

int main () {
    // client is here

    // client creates the type of notification he or she desires and give priority
    NotificationType* message= new SecurityNotification("high alert for weather");

    // now he has an instance to sender
    notificationService* sender = new HighprioritySender();

    // now the user will get the target user to whom he wants to send noti
    User target("Arjun", "arjun1234@gmail.com","9838777261");

    // now he sends noti
    sender->sendNotification(target, message);
    return 0;
}