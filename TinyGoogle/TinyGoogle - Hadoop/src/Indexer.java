import java.io.IOException;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.lang.*;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.util.*;
import org.apache.hadoop.mapreduce.*;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class Indexer extends Configured implements Tool
{

  public static class TokenizerMapper extends Mapper<LongWritable, Text, Text, Text>
  {
        //Global variables accessed across parallel maps - save state across maps for in-mapper combinining design pattern

        //private Map<String, Integer> map;
        private String pageid = null;
        private HashMap<String, Integer> fileCounterMap = null;

        /* Algorithm that uses an associative array to aggregate term counts on a per-document basis */
                public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException
                {
                        //HashMap with the partial combiners per file read
                        HashMap<String, HashMap<String, Integer>> master = new HashMap<String, HashMap<String, Integer>>();
                        if(pageid == null) {
                                pageid = ((FileSplit) context.getInputSplit()).getPath().getName();
                        }
                        //Map<String, Integer> map = getMap();
                        String line = value.toString();
			System.out.println("LINE: " + line);
                        StringTokenizer tokenizer = new StringTokenizer(line);

                        String wordPattern = "[A-Za-z]+";
                        Pattern r = Pattern.compile(wordPattern);
			int testcount = 1;

			//Map to count word occurrences per file (give id which is the filename)
                        fileCounterMap = master.get(pageid);
                        if (fileCounterMap == null){
                        	//Does not exist?  Create a map to count the word occurrences within a page
                                HashMap<String, Integer> fileCounterMapT = new HashMap<String, Integer>();
                                master.put(pageid, fileCounterMapT);
                                System.out.println("fileCounterMap does not exist. Creating map...");
                        } 
                        while (tokenizer.hasMoreTokens())
                        {
                                Matcher m = r.matcher(tokenizer.nextToken());
                                if(m.find()){
                                        String token = m.group().toLowerCase();
					
					fileCounterMap = master.get(pageid);
                                                //Exists? Start counting the words introducing a new counter per word within a page
                                                Integer counter = fileCounterMap.get(token);
                                                if (counter == null) {
                                                        fileCounterMap.put(token, 1);
							//System.out.println("Counter is null. Setting count of " + token + " to 1.");
                                                } else {
                                                        int actual = counter.intValue() + 1;
                                                        fileCounterMap.put(token, actual);
							//System.out.println("Counter exists.  Setting count of " + token + " to " + actual);
                                                }

                                        //if(map.containsKey(token)){
                                        //      int total = map.get(token).intValue() + 1;
                                        //      map.put(token, total);
                                        //} else {
                                        //      map.put(token, 1);
                                        //}
					testcount++;
                                } else {
					System.out.println("Did not match!");
				}
                        }
			//System.out.println("TESTCOUNT: " + testcount);
                        //Second phase: emit keys
                        //Per file counting map
                        Iterator<String> itCombinerFile = master.keySet().iterator();
                        while(itCombinerFile.hasNext()){
                                String fileid = itCombinerFile.next();
                                HashMap<String, Integer> countFile = master.get(fileid);

                                //and per word counting map within a file counter
                                Iterator<String> itCombiner = countFile.keySet().iterator();

                                while(itCombiner.hasNext()){
                                        String word = itCombiner.next();
                                        Integer countWord = countFile.get(word);

                                        //Emit the KV tuple <word, <file, counter>>
                                        String combinerTuple = fileid + "=" + countWord;
					//System.out.println("KEY: " + word + ", VALUE: " + combinerTuple);
                                        context.write(new Text(word), new Text(combinerTuple));
                                }
                        }

                }

                /*
                protected void cleanup(Context context) throws IOException, InterruptedException {
                        Map<String, Integer> map = getMap();
                        Iterator<Map.Entry<String, Integer>> it = map.entrySet().iterator();
                        while(it.hasNext()) {
                                Map.Entry<String, Integer> entry = it.next();
                                String sKey = entry.getKey();
                                int total = entry.getValue().intValue();
                                context.write(new Text(sKey), new IntWritable(total));
                        }
                }

                public Map<String, Integer> getMap() {
                        if(null == map)
                                map = new HashMap<String, Integer>();
                        return map;
                }
                */
        }

        public static class TokenizerReducer extends Reducer<Text, Text, Text, Text>
        {
                public void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException
                {
                        //Map to count word occurrences per file
                        HashMap<String, Integer> fileCounter = new HashMap<String, Integer>();

                        //Get all the values (files)
                        for (Text value : values) {
                                String fileCount = value.toString();
                                String[] fileCountTuple = fileCount.split("=");

                                String fileName = fileCountTuple[0];
                                Integer countForFile = Integer.parseInt(fileCountTuple[1]);

                                //Is the word in the map?
                                Integer counter = fileCounter.get(fileName);

                                // If not, initialize the counter.  Else, take the actual counter and add the partial count
                                if (counter == null) {
                                        fileCounter.put(fileName, countForFile);
                                } else {
                                        fileCounter.remove(fileName);
                                        Integer actual = counter + countForFile;
                                        fileCounter.put(fileName, actual);
                                }
                        }
                        context.write(key, new Text(fileCounter.toString()));
                        //int sum = 0;
                        /*iterates through all the values available with a key and add them together and give the
                        final result as the key and sum of its values*/
                        /*
                        for (IntWritable value : values)
                        {
                            sum += value.get();
                        }
                        context.write(key, new IntWritable(sum));       //emit(term t, count sum)
                        */
                }
        }

        public int run(String[] args) throws Exception
        {
                //getting configuration object and setting job name
                Configuration conf = getConf();
                Job job = new Job(conf, "indexer");
		        Path base = new Path(args[0]);

                //setting the class names
                job.setJarByClass(Indexer.class);

                //Allows for traversing through subdirectories and finding the files from which to extract the terms and their counts
        		FileSystem fs = FileSystem.get(conf);
        		for(FileStatus status : fs.listStatus(base)) {
        			Path pathname = status.getPath();
        			for (FileStatus inputFile : fs.listStatus(pathname)) {
        				Path inputFilePath = inputFile.getPath();
        				FileInputFormat.addInputPath(job, inputFilePath);
        			}
        		}
                FileOutputFormat.setOutputPath(job, new Path(args[1]));

                job.setMapperClass(TokenizerMapper.class);
                job.setReducerClass(TokenizerReducer.class);

                //setting the output data type classes
                job.setOutputKeyClass(Text.class);
                job.setOutputValueClass(Text.class);

                return job.waitForCompletion(true) ? 0 : 1;
        }

        public static void main(String[] args) throws Exception {
                int res = ToolRunner.run(new Configuration(), new Indexer(), args);
                System.exit(res);
        }
}

