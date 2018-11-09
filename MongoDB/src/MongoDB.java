
import com.mongodb.BasicDBObject;
import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.Mongo;
import java.util.Arrays;
import java.util.Properties;

import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.clients.consumer.ConsumerRecords;
import org.apache.kafka.clients.consumer.KafkaConsumer;

public class MongoDB {

  public static void main(String[] args) {
    Properties props = new Properties();
    props.put("bootstrap.servers", "localhost:9092");
    props.put("zk.connect", "localhost:2181");
    props.put("group.id", "groupsS");
    props.put("enable.auto.commit", "true");
    props.put("auto.commit.interval.ms", "1000");
    props.put("auto.offset.reset", "earliest");
    props.put("session.timeout.ms", "30000");
    props.put("key.deserializer", "org.apache.kafka.common.serialization.StringDeserializer");
    props.put("value.deserializer", "org.apache.kafka.common.serialization.StringDeserializer");
  
    
    KafkaConsumer<String, String> kafkaConsumer = new KafkaConsumer<>(props);
    kafkaConsumer.subscribe(Arrays.asList("testing1"));
    while (true) {
          ConsumerRecords<String, String> records = kafkaConsumer.poll(5);
        	
              for (ConsumerRecord<String, String> record : records) {
                  Mongo	mongo = new Mongo("localhost", 27017);
		          DB  db = mongo.getDB("IoTDatabase");
		          DBCollection table = null;
 	              table = db.getCollection("StreamingTable");
 	              System.out.println("Collection succesfully inserted inside the StreamingTable.");
                  BasicDBObject document = new BasicDBObject();
		          document.put("EKGDATA", record.value());
		          table.insert(document);
		          //System.out.println("Ekleme başarıyla sağlandı.");
              }
      for (ConsumerRecord<String, String> record : records) {
        System.out.println("Partition: " + record.partition() + " Offset: " + record.offset()
            + " Value: " + record.value() );
      }
    }

  }

}
