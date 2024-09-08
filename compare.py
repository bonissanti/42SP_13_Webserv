def read_file(file_path):
    """Read a file and return its binary content."""
    with open(file_path, 'rb') as file:
        return file.read()

def compare_files(file1, file2):
    """Compare two binary files."""
    return file1 == file2

def main(original_file_path, uploaded_file_path):
    try:
        # Read the original PNG file
        original_file = read_file(original_file_path)

        # Read the uploaded PNG file
        uploaded_file = read_file(uploaded_file_path)

        # Compare the two files
        if compare_files(original_file, uploaded_file):
            print("The files are identical.")
        else:
            print("The files are different.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    # Replace these paths with the actual file paths
    original_file_path = "copy.png"
    uploaded_file_path = "/home/renato/Downloads/1.png"
    
    main(original_file_path, uploaded_file_path)