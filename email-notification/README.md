# E-mail Notification of Bad Air Quality on GCP

## Prerequisites
- Set up the Air-Quality Sensing IoT System, you can follow this [tutorial](https://www.markdownguide.org/basic-syntax/).

## Step 1: Cloud Functions on GCP
Please follow [Using Pub/Sub to trigger a Cloud Function](https://cloud.google.com/scheduler/docs/tut-pub-sub), this tutorial shows us how to use Cloud Scheduler and Pub/Sub to trigger a Cloud Function. With the Cloud Function, we can integrate the code that sends e-mail inside. And here are some notes for different parts in the tutorial:

- For **Create a Cloud Function** part, please use the topic in your IoT Air-quality project. In this way, the cloud function can receive the message from the selected topic. You need to select ***Python3.8*** for the Runtime. 

- Then, please ignore the **Create a Cloud Scheduler job** part, this part is to create a cron job that sends message to Pub/Sub topic. However, our Pub/Sub topic has already received messages from our IoT device, so it's not necessary to create this cron job.

- Also ignore the **Run your job** part, since this part is to run the cron job that we don't have.

- After setting up all the above steps, you can follow the **Verify the results in Cloud Functions** part to check whether the messages are coming from Pub/Sub to the cloud function in Logs.

## Step 2: Set up Sendgrid
Sendgrid is a third-party cloud based email platform that provide us api to send e-mails. In this step, please go throuth the following steps to set up the Sendgrid service that can help us send e-mails.
1. Activate a SendGrid plan on [GCP marketplace](https://console.cloud.google.com/marketplace/details/sendgrid-app/sendgrid-email), you can use the free plan that can send 12K mails per month.
2. Create an [API Key](https://app.sendgrid.com/settings/api_keys) in Sendgrid.
3. Validate your sendgrid account email.

## Step 3: Use Sendgrid API in Cloud Function
In this part, you are going to apply Sendgrid API in the cloud function to send e-mail once you want to do noficitaion. Please go through the following steps to set up.
1. In cloud function, you need to [create a environment variable](https://cloud.google.com/functions/docs/env-var#gcloud_5) EMAIL_API_KEY with the above sendgrid api key.
    ```
    EMAIL_API_KEY = [Your Sendgrid API Key]
    ```
2. Besides, you need to add the following to [**requirements.txt**](https://cloud.google.com/functions/docs/writing/specifying-dependencies-python).
    ```
    sendgrid
    ```
3. Copy the code from [[hello_pubsub.py](https://github.com/AdrianHsu/IoT-air-quality-monitoring-on-gcp/blob/main/email-notification/hello_pubsub.py)] and paste it to the code part in you cloud function. Following are some explanations for the each part of the code.
     ### Receive Message
    ```
    pubsub_message = base64.b64decode(event['data']).decode('utf-8')
    ```
    With the above code, you can receive the message from Pub/Sub topic in a string form such as: 
    ```
    {ad_value: 23, density: 148.7598, humidity: 50, temperature: 29, timestamp: 1606114824}
    ```
    ### Process the data
    To use the above data, you need to process them and store them to an appropriate data structure. In this code, I store the processed data into a dictionary called **Info**.
    ```
    pubsub_message = pubsub_message.strip()
    pubsub_message = pubsub_message[1:-1]
    pubsub_message_info = pubsub_message.split(',')

    Info = {}

    for info in pubsub_message_info[:-1]:
        metric, value = info.split(':')
        metric = metric[1:-1]
        value = float(value)
        Info[metric] = value
    ```
     After the above code, the dictionary Info will have each item like
     ```
     {  
      'ad_value': 23.0,
      'density': ,148.7598,
      'humidity': 50,
      'temperature': 29
     }
     ```
     
     ### Set Up the E-mail Notification
     In this example, whenever the dust density is higher than 500, the recipient will receive an e-mail with the subject **Dust is too much** that notifies the user to check the indoor dust.
    ```
    if Info['density'] >= 500:

        message = Mail(

            from_email='ericlin8545@gmail.com', # The Sender

            to_emails='ericlin8545@cs.ucla.edu', # The Recipient

            subject='Dust is too much', # E-mail Subject

            html_content='<strong>You need to check the dust indoor.</strong>') # E-mail Content, HTML format

        try:

            sg = SendGridAPIClient(os.environ.get('EMAIL_API_KEY'))

            response = sg.send(message)

            print(response.status_code)

            print(response.body)

            print(response.headers)

        except Exception as e:

            print(e)
    ```

Reference: https://stackoverflow.com/questions/62282170/whats-the-best-way-to-send-an-e-mail-via-python-google-cloud-function