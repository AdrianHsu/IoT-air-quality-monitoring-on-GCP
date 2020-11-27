import base64

# using SendGrid's Python Library
# https://github.com/sendgrid/sendgrid-python
import os
from sendgrid import SendGridAPIClient
from sendgrid.helpers.mail import Mail

def hello_pubsub(event, context):
    """Triggered from a message on a Cloud Pub/Sub topic.
    Args:
         event (dict): Event payload.
         context (google.cloud.functions.Context): Metadata for the event.
    """
    pubsub_message = base64.b64decode(event['data']).decode('utf-8')

    
    pubsub_message = pubsub_message.strip()
    pubsub_message = pubsub_message[1:-1]
    pubsub_message_info = pubsub_message.split(',')

    Info = {}

    for info in pubsub_message_info[:-1]:
        metric, value = info.split(':')
        metric = metric[1:-1]
        value = float(value)
        Info[metric] = value


    if Info['density'] >= 500:
        message = Mail(
            from_email='ericlin8545@gmail.com',
            to_emails='ericlin8545@cs.ucla.edu',
            subject='Dust is too much',
            html_content='<strong>You need to check the dust indoor.</strong>')
        try:
            sg = SendGridAPIClient(os.environ.get('EMAIL_API_KEY'))
            response = sg.send(message)
            print(response.status_code)
            print(response.body)
            print(response.headers)
        except Exception as e:
            print(e)
