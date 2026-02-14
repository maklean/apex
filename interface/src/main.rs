use axum::{
    routing::get,
    Router
};

pub mod models;

#[tokio::main]
async fn main() {
    dotenvy::dotenv().expect("Failed to load environment variables."); // load env
    tracing_subscriber::fmt::init(); // enable logging system

    let app = Router::new()
        .route("/", get(root));

    // start listening on port 3000
    let listener = tokio::net::TcpListener::bind("127.0.0.1:3000")
        .await
        .expect("Failed to bind address for TCP listener.");
    
    tracing::info!(
        "Server listening on {}", 
        listener.local_addr().expect("Failed to retrieve local address.")
    );

    axum::serve(listener, app)
        .await
        .expect("Failed to serve server.");
}

async fn root() -> &'static str {
    tracing::info!("GET /");
    return "Hello, World!";
}