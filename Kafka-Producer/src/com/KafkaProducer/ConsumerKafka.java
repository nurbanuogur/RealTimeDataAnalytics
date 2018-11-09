package com.KafkaProducer;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import org.apache.kafka.clients.consumer.ConsumerConfig;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.clients.consumer.ConsumerRecords;
import org.apache.kafka.clients.consumer.KafkaConsumer;

public class ConsumerKafka {

  public static void main(String[] args) {
   
    
    
    Map<String, Object> kafkaParams = new HashMap<>();
    kafkaParams.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG,"localhost:9092");
    kafkaParams.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG, 
                    "org.apache.kafka.common.serialization.StringDeserializer");
    kafkaParams.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, 
                    "org.apache.kafka.common.serialization.StringDeserializer");
    kafkaParams.put(ConsumerConfig.GROUP_ID_CONFIG,"group1");
    kafkaParams.put(ConsumerConfig.AUTO_OFFSET_RESET_CONFIG,"latest");
    kafkaParams.put(ConsumerConfig.ENABLE_AUTO_COMMIT_CONFIG,true);

    //Configure Spark to listen messages in topic test
  
    KafkaConsumer<String, String> kafkaConsumer = new KafkaConsumer<>(kafkaParams);
    kafkaConsumer.subscribe(Arrays.asList("p1"));
    while (true) {
      ConsumerRecords<String, String> records = kafkaConsumer.poll(100);
      for (ConsumerRecord<String, String> record : records) {
        System.out.println("Partition: " + record.partition() + " Offset: " + record.offset()
            + " Value: " + record.value());
      }
    }

  }

}
