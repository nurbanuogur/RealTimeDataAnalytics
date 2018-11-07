package com.SparkStreaming;

import org.apache.kafka.clients.consumer.ConsumerConfig;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.spark.SparkConf;
import org.apache.spark.SparkContext;
import org.apache.spark.api.java.JavaRDD;
import org.apache.spark.api.java.JavaSparkContext;
import org.apache.spark.sql.Dataset;
import org.apache.spark.sql.Row;
import org.apache.spark.streaming.Durations;
import org.apache.spark.streaming.StreamingContext;
import org.apache.spark.streaming.api.java.JavaDStream;
import org.apache.spark.streaming.api.java.JavaInputDStream;
import org.apache.spark.streaming.api.java.JavaStreamingContext;
//import org.apache.spark.streaming.kafka.*;
import org.apache.spark.streaming.kafka010.*;
import org.apache.spark.sql.RowFactory;
import org.apache.spark.sql.SQLContext;
import java.util.HashMap;
import java.util.Map;
import org.apache.spark.api.java.function.Function;
import org.apache.spark.ml.classification.LogisticRegression;
import org.apache.spark.ml.classification.LogisticRegressionModel;
import org.apache.spark.ml.evaluation.MulticlassClassificationEvaluator;
import org.apache.spark.ml.feature.StringIndexer;
import org.apache.spark.ml.feature.VectorAssembler;
import org.apache.spark.sql.types.DataTypes;
import org.apache.spark.sql.types.StructField;
import org.apache.spark.sql.types.StructType;
import org.apache.spark.sql.types.Metadata;
import java.util.Arrays;
import java.util.List;
import static org.apache.spark.sql.types.DataTypes.*;
import java.util.*;

/**
* Created by sunilpatil on 1/11/17.
*/
public class Streaming{
   public static void main(String[] args) throws Exception{

  
       // Configure Spark to connect to Kafka running on local machine
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
       //  Collection<String> topics = Arrays.asList("bc");
       Collection<String> topics = Arrays.asList("testing1");
       
       // Create a local StreamingContext with two working thread and batch interval of 5 second
       SparkConf conf = new SparkConf().setAppName("LogisticRegressionClassifier")
               .setMaster("local[*]").set("spark.executor.memory","2g"); 

       
       SparkContext sp=new SparkContext(conf);
       
       StreamingContext ssc=new StreamingContext(sp,Durations.seconds(1));
      
       JavaStreamingContext jssc = new JavaStreamingContext(ssc);
           
       JavaSparkContext jsccc  =  jssc.sparkContext();
       SQLContext sqlContext = new SQLContext(jsccc);
   
       Model oModel=new Model();
       List<Row> data =oModel.Modelw();
       
       StructType schema1 = new StructType(new StructField[]{
    		   
       		  new StructField("Column", DataTypes.IntegerType, false, Metadata.empty()),
       		  new StructField("patientID", DataTypes.IntegerType, false, Metadata.empty()),
       		  new StructField("Age", DataTypes.IntegerType, false, Metadata.empty()),
       		  new StructField("Gender", DataTypes.IntegerType, false, Metadata.empty()),
       		  new StructField("Height", DataTypes.IntegerType, false, Metadata.empty()),
       		  new StructField("Weight", DataTypes.IntegerType, false, Metadata.empty()),
       		  new StructField("QRS", DataTypes.IntegerType, false, Metadata.empty()),
       		  new StructField("P-R", DataTypes.IntegerType, false, Metadata.empty()),
       		});
       
       Dataset<Row> sentenceData = sqlContext.createDataFrame(data, schema1);
       
      // sentenceData.show();
       
       

       VectorAssembler assembler1 = new VectorAssembler()

               .setInputCols(new String[] {"QRS","P-R"})

               .setOutputCol("features");

               Dataset<Row> finalDS1 = assembler1.transform(sentenceData);

              // finalDS1.show();
               
               
               StringIndexer labelIndexer = new StringIndexer().setInputCol("Column").setOutputCol("label");
    		   
               Dataset<Row> satir = labelIndexer.fit(finalDS1).transform(finalDS1);
               
              // satir.show();
               
       // Start reading messages from Kafka and get DStream
       final JavaInputDStream<ConsumerRecord<String, String>> stream =
       		 
               KafkaUtils.createDirectStream(jssc, LocationStrategies.PreferConsistent(), 
                                             ConsumerStrategies.<String,String>Subscribe(topics,kafkaParams));


       
       // Read value of each message from Kafka and return it
       JavaDStream<String> lines = stream.map(new Function<ConsumerRecord<String,String>, String>() {
           @Override
           public String call(ConsumerRecord<String, String> kafkaRecord) throws Exception {
               return kafkaRecord.value();
           }
       });
    
       
       
       lines.foreachRDD(rdd -> {
       	if(!rdd.isEmpty()){
       		
              		
       		  JavaRDD<Row> rowRDD = rdd.map((Function<String, Row>) record -> {
       		      String[] attributes = record.split(",");
         		      return RowFactory.create(Integer.valueOf(attributes[0]), Integer.valueOf(attributes[1]), Integer.valueOf(attributes[2]),Integer.valueOf(attributes[3]),Integer.valueOf(attributes[4]),Integer.valueOf(attributes[5]),Integer.valueOf(attributes[6]),Integer.valueOf(attributes[7]));
       		  });
       	
       		  
       		  StructType schema = createStructType(new StructField[]{

         			    createStructField("Column", IntegerType, false),
         			    createStructField("PatientID", IntegerType, false),
         			    createStructField("Age", IntegerType, false),
         			    createStructField("Gender", IntegerType, false),
         			    createStructField("Height", IntegerType, false),
         			    createStructField("Weight", IntegerType, false),
         			    createStructField("QRS", IntegerType, false),
         			    createStructField("P-R", IntegerType, false)
         			

         			    });
       		  
       		  	// SparkSession spark = JavaSparkSessionSingleton.getInstance(rdd.context().getConf());
                   Dataset<Row> msgDataFrame = sqlContext.createDataFrame(rowRDD, schema).toDF();
                 //  msgDataFrame.show();
            

                   VectorAssembler assembler = new VectorAssembler()

                           .setInputCols(new String[] {"QRS","P-R"})

                           .setOutputCol("features");

                           Dataset<Row> finalDS = assembler.transform(msgDataFrame);

                           //Dataset<Row>[] splits = finalDS.randomSplit(new double[] { 0.7, 0.3 });

                          //   Dataset<Row> trainingData = splits[0];

                         //  Dataset<Row> testData = splits[1];

                         //  trainingData.show();

                       //    testData.show();
                       //    finalDS.show();
                           
                           
                          StringIndexer labelIndexer1 = new StringIndexer().setInputCol("Column").setOutputCol("label");
                        		   
                          Dataset<Row> satir1 = labelIndexer1.fit(finalDS).transform(finalDS);
                       //   satir1.show();
                           
   				    LogisticRegression lr = new LogisticRegression().setMaxIter(10).setRegParam(0.3).setElasticNetParam(0.8);
   				   // System.out.println("LogisticRegression parameters:\n" + lr.explainParams() + "\n");
   				    
   				
   			        LogisticRegressionModel lrModel = lr.fit(satir);
   			       // System.out.println("Model 1 was fit using parameters: " + lrModel.parent().extractParamMap());

   			        // Transform the model, and predict class for test dataset

   			        Dataset<Row> predictions = lrModel.transform(satir1);

   			    predictions.show();
                   
   			       				  
   				 // obtain evaluator.
   			     
   				 MulticlassClassificationEvaluator evaluator = new MulticlassClassificationEvaluator().setLabelCol("label").setPredictionCol("prediction")
   						.setMetricName("accuracy");

   				 // compute the classification error on test data.
   				 double accuracy = evaluator.evaluate(predictions);
   				
   				// output.select("patientID","label","prediction").show();
   				 
   				 System.out.println("Test Error = " + (1 - accuracy));
   				 System.out.println("Model Accuracy = " +  accuracy);
   			  
               }
           });
           	
     
      
           jssc.start();
           jssc.awaitTermination();
       }
       }
                   
                   
       

               