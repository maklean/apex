fn main() {
    // makes it search for libraries at /lib
    println!("cargo:rustc-link-search=native=../lib");

    // telemetry static lib
    println!("cargo:rustc-link-lib=static=apex-telemetry");
}