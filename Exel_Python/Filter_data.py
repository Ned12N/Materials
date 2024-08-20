import pandas as pd

# File path and sheet details
input_file = 'test_data.xlsx'
start_row = 1  # Start reading from this row

filtered_sheets = {}

xl = pd.ExcelFile(input_file)
for sheet_name in xl.sheet_names:
    df = xl.parse(sheet_name, skiprows=start_row)

    # Specify the column and filter values
    filter_column_index = 0  # Assuming column A is the column to filter on (using index 0 for the first column)
    filter_values = ['switch1', 'switch2']

    # Manually filter the data
    filtered_rows = []

    # Iterate over each row and check if the filter value matches
    for index, row in df.iterrows():
        if row.iloc[filter_column_index] in filter_values:
            filtered_rows.append(row)

    # Create a new DataFrame from the filtered rows
    filtered_df = pd.DataFrame(filtered_rows)

    # Store the filtered DataFrame in the dictionary
    filtered_sheets[sheet_name] = filtered_df

# Define output file
output_file = 'filtered_data.xlsx'

# Save all filtered sheets to the output Excel file
with pd.ExcelWriter(output_file) as writer:
    for sheet_namex, value in filtered_sheets.items():
        # Save each filtered DataFrame to its corresponding sheet
        value.to_excel(writer, sheet_name=sheet_namex, index=False)

print(f"Filtered data saved to {output_file}")
