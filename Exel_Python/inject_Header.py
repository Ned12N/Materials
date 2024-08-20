import pandas as pd

# File path details
input_file = 'test_data.xlsx'
output_file = 'updated_test_data.xlsx'

# Load the Excel file
xl = pd.ExcelFile(input_file)

# Define the new header row you want to add
new_header = ['NewHeader1', 'NewHeader2', 'NewHeader3', 'NewHeader4', 'NewHeader5']  # Adjust this list based on your actual columns

# Loop through each sheet and add the new header row
modified_sheets = {}

for sheet_name in xl.sheet_names:
    # Parse the sheet into a DataFrame
    df = xl.parse(sheet_name, header=None)  # Do not treat the first row as the header

    # Insert the new header at the first row
    new_header_df = pd.DataFrame([new_header])

    # Concatenate the new header with the existing DataFrame
    df_with_new_header = pd.concat([new_header_df, df], ignore_index=True)

    # Store and write the updated DataFrame back to Excel
    with pd.ExcelWriter(output_file, engine='openpyxl', mode='a' if sheet_name != xl.sheet_names[0] else 'w') as writer:
        df_with_new_header.to_excel(writer, sheet_name=sheet_name, index=False, header=False)


print(f"New header row added and saved to {output_file}")
