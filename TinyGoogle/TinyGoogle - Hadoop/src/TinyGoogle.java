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

public class TinyGoogle extends Configured implements Tool
{
	//Global variables

	public static class TinyGoogleMapper extends Mapper<LongWritable, Text, Text, Text>
	{
		public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException
		{
			Configuration conf = context.getConfiguration();
			String terms = conf.get("searchterms");
			List<String> list = Arrays.asList(terms.substring(1, terms.length() - 1).split(", "));
			
			String line = value.toString();
			System.out.println("LINE: " + line);
			String[] splitline = line.split("(?=[{])");
			System.out.println(splitline[0]);

			System.out.println("SEARCH TERMS: " + list.toString());
			for (String s : list){
				//System.out.println("Coming in the for loop.... " + s);
				if(splitline[0].trim().equalsIgnoreCase(s.trim())){
					System.out.println("YAY! " + splitline[0] + " is the same as " + s);
					context.write(new Text(s), new Text(splitline[1]));
				}
			}	
		}	
	}

	public static class TinyGoogleReducer extends Reducer<Text, Text, Text, Text>
	{
		public void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException
		{
			for (Text value : values) {
				System.out.println("REDUCER: " + key.toString() + ", " + value.toString());
				context.write(key, value);
			}
		}
	}

	public int run(String[] args) throws Exception
	{
		Configuration conf = getConf();
		boolean found_search_terms = false;
		List<String> search_terms = new ArrayList<String>();
	
		for (int i=0; i < args.length; ++i) {
			try {
				if(found_search_terms){
                                        search_terms.add(args[i]);
					System.out.println("Adding " + args[i] + " to search_terms.");
                                }
				if ("-search".equals(args[i])) {
					found_search_terms = true;
					System.out.println("Found search terms.");
				}
			} catch (ArrayIndexOutOfBoundsException except) {
				System.out.println("ERROR: Required parameter mising from " + args[i-1]);
				System.out.println("Usage: TinyGoogle <input> <output> -search [query terms]");
			}
		}
		conf.set("searchterms", search_terms.toString());
		Job job = new Job(conf, "tinygoogle");
		String base = args[0] + "/part-r-00000";

		Path basePath = new Path(base);
		
		job.setJarByClass(TinyGoogle.class);

		FileInputFormat.addInputPath(job, basePath);
		FileOutputFormat.setOutputPath(job, new Path(args[1]));

		job.setMapperClass(TinyGoogleMapper.class);
		job.setReducerClass(TinyGoogleReducer.class);

		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);

		return job.waitForCompletion(true) ? 0 : 1;
	}

	public static void main(String[] args) throws Exception {
		int res = ToolRunner.run(new Configuration(), new TinyGoogle(), args);
		System.exit(res);
	}
}
