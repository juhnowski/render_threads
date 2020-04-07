import pika
import sys
import time

if __name__ == '__main__':

    topic_name = 'topic_golive-renderer'
    routing_key = 'golive-renderer.command'
    message_master = '{"command":"add","params":{"id":"master","audio":50051,"video":50041,"rtmp":"rtmp://gpu3.view.me/live/test890"}}'
    message_slave = '{"command":"add","params":{"id":"slave","audio":51051,"video":51041,"rtmp":"rtmp://gpu3.view.me/live/test890"}}'
    message_start = '{"command":"start","params":{}}'
    message_stop = '{"command":"stop","params":{}}'
    connection = pika.BlockingConnection(
        pika.ConnectionParameters(host='localhost'))
    channel = connection.channel()

    channel.exchange_declare(exchange=topic_name, exchange_type='topic')

    channel.basic_publish(
        exchange=topic_name, routing_key=routing_key, body=message_master)
    print(" [x] Sent %r:%r" % (routing_key, message_master))

    channel.basic_publish(
        exchange=topic_name, routing_key=routing_key, body=message_slave)
    print(" [x] Sent %r:%r" % (routing_key, message_slave))

    channel.basic_publish(
        exchange=topic_name, routing_key=routing_key, body=message_start)
    print(" [x] Sent %r:%r" % (routing_key, message_start))

    time.sleep(3)  # Сон в 3 секунды

    channel.basic_publish(
        exchange=topic_name, routing_key=routing_key, body=message_stop)
    print(" [x] Sent %r:%r" % (routing_key, message_stop))

    connection.close()