import re
from pathlib import Path
from multiprocessing import Pool
import subprocess  # Import subprocess to execute shell commands
import os

class OpenFOAMReconstructor:
    
    def __init__(self, case_directory, num_processors, excluded_time_steps=None, included_time_steps=None):
        
        self.case_directory = Path(case_directory)
        
        self.num_processors = num_processors
        
        self.excluded_time_steps = excluded_time_steps if excluded_time_steps is not None else []
        
        self.included_time_steps = included_time_steps if included_time_steps is not None else []
        
    def check_decomposed(self):
        
        processor_dirs = list(self.case_directory.glob("processor*"))
        
        if processor_dirs:
            print(f"Decomposed case detected with {len(processor_dirs)} processor directories.")
            return True
        else:
            print("No decomposed case detected.")
            return False
    
    def get_time_steps(self):
        
        processor0_path = self.case_directory / "processor0"
        
        if processor0_path.exists():
            all_time_steps = [dir.name for dir in processor0_path.iterdir() if dir.is_dir() and re.match(r"^\d+(\.\d+)?$", dir.name)]
            # Filter based on inclusion and exclusion lists
            if self.included_time_steps:
                # Prioritize included time steps
                time_steps = [ts for ts in self.included_time_steps if ts in all_time_steps and ts not in self.excluded_time_steps]
            else:
                # Apply exclusion list if no included time steps are specified
                time_steps = [ts for ts in all_time_steps if ts not in self.excluded_time_steps]
            print(f"Time steps considered for reconstruction: {time_steps}")
            return time_steps
        else:
            print("processor0 directory does not exist.")
            return []

    
    def reconstruct_parallel(self, time_steps):
        # Split time_steps for N processors
        chunks = [time_steps[i::self.num_processors] for i in range(self.num_processors)]
        
        with Pool(self.num_processors) as pool:
            pool.map(self.reconstruct_for_time_step, chunks)
    
    def reconstruct_for_time_step(self, time_steps_chunk):
        # Construct the time range string for the reconstructPar command
        time_range = ','.join(time_steps_chunk)
        command = f"reconstructPar -newTimes -time {time_range}"
        
        print(f"Executing: {command}")
        try:
            # Execute the command in the case directory
            subprocess.run(command, shell=True, check=True, cwd=self.case_directory)
        except subprocess.CalledProcessError as e:
            print(f"Error during reconstruction: {e}")
            
    def print_intention(self):
        
        print("Program Intention Summary:")
        print(f"Case Directory: {self.case_directory}")
        print(f"Number of Processors for Reconstruction: {self.num_processors}")
        if self.excluded_time_steps:
            excluded_steps_str = ", ".join(self.excluded_time_steps)
            print(f"Excluded Time Steps: {excluded_steps_str}")
        else:
            print("No Time Steps Excluded.")

        if self.included_time_steps:
            included_steps_str = ", ".join(self.included_time_steps)
            print(f"Included Time Steps: {included_steps_str}")
        else:
            print("All Time Steps Included, except any excluded.")

        self.check_decomposed()
        
        self.get_time_steps()

    
    def run(self):
        
        if self.check_decomposed():
            time_steps = self.get_time_steps()
            if time_steps:
                self.reconstruct_parallel(time_steps)


if __name__ == "__main__":
        
    excluded_time_steps = []
    
    included_time_steps = []
    
    case_directory = os.getcwd()
    
    num_processors = 28  # Adjust based on your hardware or requirements
    
    reconstructor = OpenFOAMReconstructor(case_directory, num_processors, excluded_time_steps, included_time_steps)

    reconstructor.print_intention()
    
    reconstructor.run()


 
