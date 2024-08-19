import pandas as pd
import openpyxl
from openpyxl.styles import Alignment

def sort_and_save_sheets(input_file, output_file):
    """
    Reads an Excel file, sorts each sheet by all column headers,
    and saves the sorted sheets to a new Excel file.

    Parameters:
    input_file (str): Path to the input Excel file.
    output_file (str): Path to the output Excel file.
    """
    sorted_sheets = {}

    # Read the Excel file
    xl = pd.ExcelFile(input_file)

    # Loop through each sheet 
    for sheet_name in xl.sheet_names:
        df = xl.parse(sheet_name)

        # Extract and print column headers dynamically
        columns_headers = [col for col in df.columns]
        print(f"Headers of current sheet '{sheet_name}' = ", columns_headers)

        # Sort the entire DataFrame by Header Names (one by one)
        sorted_df = df.sort_values(by=columns_headers)

        # Store the sorted DataFrame in the dictionary
        sorted_sheets[sheet_name] = sorted_df
        

    # Save all sorted sheets back to the output Excel file
    with pd.ExcelWriter(output_file) as writer:
        for sheet, sorted_data in sorted_sheets.items():
            sorted_data.to_excel(writer, sheet_name=sheet, index=False)

    print(f"Sorted data saved to {output_file}")

def autofit_columns_and_rows(excel_file):
    """
    Applies AutoFit to all columns and rows in all sheets of the given Excel file
    and centers the content within each cell.

    Parameters:
    excel_file (str): Path to the Excel file to be adjusted.
    """
    # Load the workbook and iterate through each sheet
    wb = openpyxl.load_workbook(excel_file)
    for sheet in wb.worksheets:
        # AutoFit for columns
        for column_cells in sheet.columns:
            max_length = 0
            column = column_cells[0].column_letter  # Get the column name
            for cell in column_cells:
                try:
                    # Calculate the maximum length of the column content
                    if len(str(cell.value)) > max_length:
                        max_length = len(str(cell.value))
                    
                    # Center the content within the cell
                    cell.alignment = Alignment(horizontal='center', vertical='center')
                except:
                    pass
            adjusted_width = (max_length + 2)
            sheet.column_dimensions[column].width = adjusted_width
        
        # AutoFit for rows (optional)
        for row_cells in sheet.iter_rows():
            max_height = 15  # Set a default row height (if needed)
            for cell in row_cells:
                # Adjust row height based on the font size or other factors if required
                # Currently, this example sets the alignment only.
                cell.alignment = Alignment(horizontal='center', vertical='center')

            # You can adjust row height if necessary:
            # sheet.row_dimensions[cell.row].height = adjusted_height

    # Save the workbook with the AutoFit and alignment applied
    wb.save(excel_file)
    print(f"AutoFit and centering applied to all columns and rows in {excel_file}")

############################## Main ##############################

input_file = 'test_data.xlsx'
output_file = 'sorted_test_data.xlsx'

sort_and_save_sheets(input_file, output_file)

autofit_columns_and_rows(output_file)
