import argparse
import subprocess
import os
import pandas as pd
import pyarrow.parquet as pq
import duckdb

def run_parser(parser_executable, pcap_file):
    try:
        subprocess.run([parser_executable, pcap_file], check=True)
        print(f"Successfully ran {parser_executable} on {pcap_file}")
    except subprocess.CalledProcessError as e:
        print(f"Error running parser: {e}")
        exit(1)

def csv_to_parquet(csv_file, parquet_file):
    try:
        # Read CSV
        df = pd.read_csv(csv_file)
        
        # Write Parquet
        df.to_parquet(parquet_file)
        print(f"Successfully converted {csv_file} to {parquet_file}")
    except Exception as e:
        print(f"Error converting to parquet: {e}")
        exit(1)

def execute_sql_query(parquet_file, query_file):
    try:
        # Read the query from file
        with open(query_file, 'r') as f:
            query = f.read().strip()
        
        # Connect to DuckDB in-memory database
        conn = duckdb.connect()
        
        # Register the Parquet file as a table
        conn.execute(f"CREATE TABLE data AS SELECT * FROM '{parquet_file}'")
        
        # Execute the query
        result = conn.execute(query).fetchdf()
        
        print("\nQuery Results:")
        print(result)
        return result
    except Exception as e:
        print(f"Error executing SQL query: {e}")
        exit(1)

def main():
    parser = argparse.ArgumentParser(description='Process PCAP file and analyze results')
    parser.add_argument('parser_executable', help='Path to the C++ parser executable')
    parser.add_argument('pcap_file', help='Path to the PCAP file to analyze')
    parser.add_argument('output_dir', help='Directory containing output CSV files')
    parser.add_argument('target_csv', help='Name of the CSV file to convert to parquet')
    parser.add_argument('query_file', help='File containing SQL query to execute')
    
    args = parser.parse_args()

    # Construct full paths
    csv_path = os.path.join(args.output_dir, args.target_csv)
    parquet_path = os.path.join(args.output_dir, os.path.splitext(args.target_csv)[0] + '.parquet')
    
    run_parser(args.parser_executable, args.pcap_file)
    
    csv_to_parquet(csv_path, parquet_path)
    
    execute_sql_query(parquet_path, args.query_file)

if __name__ == '__main__':
    main()