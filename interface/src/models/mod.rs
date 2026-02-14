#[repr(C)]
#[derive(Debug)]
pub struct SystemVitals {
    cpu_usage_percentage: f64,
    total_mem_kb: u64,
    free_mem_kb: u64,
    available_mem_kb: u64,
    uptime_seconds: u64,
    network_interface_count: u16,
}

impl SystemVitals {
    pub fn empty() -> Self {
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