/*
 * chewing-editor: Chewing userphrase editor
 * Copyright (C) 2014 Chewing Development Team

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "CommitHistoryUploader.h"

#include <QDebug>
#include <QFile>
#include <curl/curl.h>

#include <cstdio>

CommitHistoryUploader::CommitHistoryUploader(QObject *parent)
    :ctx_{chewing_new2(nullptr, nullptr, nullptr, nullptr), chewing_delete}
{
}

bool CommitHistoryUploader::upload()
{
    CURL *curl;
    CURLcode res;

    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;

    QTemporaryFile file;

    if (file.open()) {
        chewing_commit_history_export(ctx_.get(), file.fileName().toStdString().c_str());

        curl_global_init(CURL_GLOBAL_ALL);

        printf("filename: %s\n", file.fileName().toStdString().c_str());
        /* Fill in the file upload field */ 
        curl_formadd(&formpost,
                     &lastptr,
                     CURLFORM_COPYNAME, "file",
                     CURLFORM_FILE, file.fileName().toStdString().c_str(),
                     CURLFORM_END);

        curl = curl_easy_init();

        if (!curl)
            return false;

        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost/addall.php");
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        curl_easy_cleanup(curl);
        curl_formfree(formpost);

        file.close();
    }
    else {
        qDebug() << "Cannot open file.";
        return false;
    }

    return true;
}
