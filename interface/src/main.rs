use std::ffi::CStr;
use std::os::raw::c_char;

#[link(name="apex-telemetry", kind="static")]
unsafe extern "C" {
    fn get_kernel_version(buffer: *mut c_char, size: usize) -> i32;
    fn calculate_cpu_usage(cpu_usage: *mut f64, time_ms: i32) -> i32;
}

fn main() {
    let mut buffer: [u8; 512] = [0; 512]; // create 512 bytes array

    let buffer_ptr: *mut c_char = buffer.as_mut_ptr() as *mut c_char; // need to convert into array of c_char to read string
    let result_code;

    // read kernel version into buffer_ptr
    unsafe { result_code = get_kernel_version(buffer_ptr, 512) };

    // TODO: import codes from telemetry.h
    if result_code != 0 {
        println!("Error: {result_code}");
        return;
    }

    let s = CStr::from_bytes_until_nul(&buffer).expect("Buffer was not null-terminated");
    println!(
        "Result Code: {result_code}\n\nKernel Version: {}", 
        s.to_str().expect("Failed to convert CStr to str")
    );

    let mut cpu_usage: f64 = -1f64;
    let time_ms = 2000;

    unsafe { calculate_cpu_usage(&mut cpu_usage as *mut f64, time_ms) };
    
    println!("CPU Usage ({}s): {cpu_usage:.2}%", time_ms / 1000);
}
