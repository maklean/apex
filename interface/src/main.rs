use std::ffi::CStr;
use std::os::raw::c_char;

pub struct SystemVitals {
    cpu_usage_percentage: f64,
    total_mem_kb: u64,
    free_mem_kb: u64,
    available_mem_kb: u64,
    uptime_seconds: u64,
    network_interface_count: u16,
}

impl SystemVitals {
    fn make_empty() -> Self {
        Self {
            cpu_usage_percentage: 0.0,
            total_mem_kb: 0,
            free_mem_kb: 0,
            available_mem_kb: 0,
            uptime_seconds: 0,
            network_interface_count: 0
        }
    }
}

#[link(name="apex-telemetry", kind="static")]
unsafe extern "C" {
    fn get_kernel_version(buffer: *mut c_char, size: usize) -> i32;
    fn collect_vitals(vitals: *mut SystemVitals) -> i32;
}

fn main() {
    let mut buffer: [u8; 512] = [0; 512]; // create 512 bytes array

    let buffer_ptr: *mut c_char = buffer.as_mut_ptr() as *mut c_char; // need to convert into array of c_char to read string
    let mut result_code;

    // read kernel version into buffer_ptr
    unsafe { result_code = get_kernel_version(buffer_ptr, 512) };

    // TODO: import codes from telemetry.h
    if result_code != 0 {
        println!("Error: {result_code}");
        return;
    }

    let s = CStr::from_bytes_until_nul(&buffer).expect("Buffer was not null-terminated");
    println!(
        "get_kernel_version() Result Code: {result_code}\n\nKernel Version: {}", 
        s.to_str().expect("Failed to convert CStr to str")
    );

    let mut vitals = SystemVitals::make_empty();
    unsafe {
        result_code = collect_vitals(&mut vitals as *mut SystemVitals);
    };

    println!("\ncollect_vitals() Result Code: {result_code}\n");
    print_system_vitals(&vitals);
}

fn print_system_vitals(vitals: &SystemVitals) {
    println!("=== System Vitals ===");
    println!("CPU Usage:           {:.2}%", vitals.cpu_usage_percentage);
    println!("Total Memory:        {} KB", vitals.total_mem_kb);
    println!("Free Memory:         {} KB", vitals.free_mem_kb);
    println!("Available Memory:    {} KB", vitals.available_mem_kb);
    println!("Uptime:              {} seconds", vitals.uptime_seconds);
    println!("Network Interfaces:  {}", vitals.network_interface_count);
    println!("=====================");
}
