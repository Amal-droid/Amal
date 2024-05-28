#include <iostream>
#include <httplib.h>
#include <curl/curl.h>

// Check for empty fields and validate email
bool validateFields(const std::string& name, const std::string& email, const std::string& phone, const std::string& message) {
    if (name.empty() || email.empty() || phone.empty() || message.empty()) {
        std::cout << "No arguments provided!\n";
        return false;
    }
    // Email validation logic here
    return true;
}

// Send email using libcurl
bool sendEmail(const std::string& name, const std::string& email, const std::string& phone, const std::string& message) {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        // Construct email message
        std::string email_subject = "Website Contact Form: " + name;
        std::string email_body = "You have received a new message from your website contact form.\n\n"
                                "Here are the details:\n\nName: " + name + "\n\nEmail: " + email +
                                "\n\nPhone: " + phone + "\n\nMessage:\n" + message;

        // Set email headers
        struct curl_slist *recipients = NULL;
        recipients = curl_slist_append(recipients, "To: amalpp42@gmail.com");
        recipients = curl_slist_append(recipients, "From: noreply@yourdomain.com");
        recipients = curl_slist_append(recipients, "Subject: " + email_subject);

        // Set email body
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.yourprovider.com");
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "<noreply@yourdomain.com>");
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READDATA, email_body.c_str());
        res = curl_easy_perform(curl);
        curl_slist_free_all(recipients);
        if (res != CURLE_OK)
            std::cerr << "Email sending failed: " << curl_easy_strerror(res) << std::endl;
        else
            std::cout << "Email sent successfully!\n";
        curl_easy_cleanup(curl);
        return true;
    } else {
        std::cerr << "Failed to initialize libcurl\n";
        return false;
    }
}

int main() {
    httplib::Server svr;

    svr.Post("/", [](const httplib::Request& req, httplib::Response& res) {
        std::string name = req.get_param_value("name");
        std::string email = req.get_param_value("email");
        std::string phone = req.get_param_value("phone");
        std::string message = req.get_param_value("message");

        if (validateFields(name, email, phone, message)) {
            if (sendEmail(name, email, phone, message)) {
                res.set_content("Email sent successfully!", "text/plain");
            } else {
                res.set_content("Failed to send email", "text/plain");
            }
        } else {
            res.set_content("Validation failed", "text/plain");
        }
    });

    svr.listen("localhost", 8080);

    return 0;
}
