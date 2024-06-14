use axum::{
    extract::{
        ws::{Message, WebSocket, WebSocketUpgrade},
        State,
    },
    http::{header, StatusCode},
    response::{IntoResponse, Json,},
    routing::{get, post},
    Router,
};
//use axum_macros::debug_handler;
use futures_util::stream::{SplitStream, SplitSink};
use serde::{Deserialize, Serialize,};
use std::net::SocketAddr;
use std::{
    //collections::HashSet,
    sync::{Arc, Mutex},
};
use tokio;
use tokio::sync::broadcast;
use futures::{sink::SinkExt, stream::StreamExt,};
use tower_http::cors::{Any, CorsLayer};
use sha256::digest;
use async_recursion::async_recursion;

#[derive(Serialize)]
struct ErrorCode {
    status: String,
}

#[derive(Serialize, Deserialize)]
struct SimpleInput {
    input: String,
}

#[derive(Eq, PartialEq, Hash, Serialize, Deserialize, Clone)]
struct UserLookup { 
    username: String,
    // todo -- add password
    password: String,
}

#[derive(Serialize, Deserialize)]
struct UserMessage { 
    username: String,
    message: String,
}

struct AppState {
    user_name: Mutex<Vec<String>>,
    user_pass: Mutex<Vec<String>>,
    tx: broadcast::Sender<String>,
}

#[tokio::main]
async fn main() {
    let cors = CorsLayer::new()
        .allow_methods([axum::http::Method::GET, axum::http::Method::POST])
        .allow_origin(Any)
        .allow_headers([header::CONTENT_TYPE]);

    let user_name = Mutex::new(Vec::new());

    let user_pass = Mutex::new(Vec::new());

    let (tx, _rx) = broadcast::channel(100);

    let app_state = Arc::new(AppState { user_name, user_pass, tx });

    let app = Router::new()
        .route("/", get(handler))
        .route("/", post(post_handler))
        .route("/register-user", post(handle_register))
        .route("/login-user", post(handle_login))
        .route("/socket", get(socket_handler))
        .layer(cors)
        .with_state(app_state);
    let addr: SocketAddr = SocketAddr::from(([127, 0, 0, 1], 8080));

    axum::Server::bind(&addr)
        .serve(app.into_make_service())
        .await
        .unwrap();
}

async fn handler() -> impl IntoResponse {
    let status = ErrorCode {
        status: String::from("success"),
    };
    (StatusCode::OK, Json(status))
}
async fn post_handler(Json(payload): Json<SimpleInput>) -> impl IntoResponse {
    let new_payload = SimpleInput {
        input: payload.input.chars().rev().collect::<String>(),
    };

    (StatusCode::OK, Json(new_payload))
}


async fn handle_register(
    State(state) : State<Arc<AppState>>,
    Json(payload) : Json<UserLookup>
) -> impl IntoResponse {
    let hashed_empty_string: String = String::from("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    let mut user_names = state.user_name.lock().unwrap();
    let mut user_passes = state.user_pass.lock().unwrap();
    let hashed_payload = UserLookup { username: payload.username, password: digest(payload.password) }; 
    println!("{} {}", hashed_payload.username, hashed_payload.password);
    if user_names.contains(&hashed_payload.username) && (hashed_payload.username != String::new() || hashed_payload.password != hashed_empty_string) {
        return (StatusCode::OK, Json(UserLookup { username: "".to_string(), password: "".to_string() }));
    }
    user_names.push(hashed_payload.username.clone());
    user_passes.push(hashed_payload.password.clone());
    (StatusCode::OK, Json(UserLookup { username: hashed_payload.username, password: digest(hashed_payload.password)}))
    // todo - impliment database magic
}


async fn handle_login(State(state): State<Arc<AppState>>, Json(payload) : Json<UserLookup>) -> impl IntoResponse {

    let payload = UserLookup { username: payload.username, password: digest(payload.password) };
    let mut name = UserLookup { username: String::new(), password: String::new() };

    get_valid_username(&state, &mut name, &payload, false);


    let final_json = UserLookup { username: name.username, password: digest(name.password) };
    println!("{} {}", final_json.username, final_json.password);
    (StatusCode::OK, Json(final_json))
}



async fn socket_handler(
    ws: WebSocketUpgrade,
    State(state): State<Arc<AppState>>
) -> impl IntoResponse {
    //println!("{} : {}", state.user, state.message);
    ws.on_upgrade(|socket| handle_socket(socket, state))
}

async fn handle_socket(socket: WebSocket, state: Arc<AppState>) {
    let (mut s, mut r) = socket.split();

     
    let mut name: UserLookup = UserLookup { username: String::new(), password: String::new() };
    let _ = verify_user(&state, &mut name, &mut r, &mut s).await;

    let mut rx = state.tx.subscribe();

    if name.username != String::new() {
        let joined = format!("{uname} joined!", uname = name.username);
        println!("{}", joined);
        let _ = state.tx.send(joined);
    } else {
        return;
    }
   

    let mut s_thread = tokio::spawn(async move {
        while let Ok(msg) = rx.recv().await {
            if s.send(Message::Text(msg)).await.is_err() {
                break;
            }
        }
    });

    let tx = state.tx.clone();
    let uname = name.username.clone();

    let mut r_thread = tokio::spawn(async move {
        while let Some(Ok(Message::Text(text))) = r.next().await {
            let user_message = UserMessage {
                username : uname.clone(),
                message: text,
            };
            println!("{}", serde_json::to_string(&user_message).unwrap());
            let _ = tx.send(serde_json::to_string(&user_message).unwrap());
    }});

    tokio::select! {
         _ = (&mut r_thread) => s_thread.abort(),
         _ = (&mut s_thread) => r_thread.abort(),
    };

    if name.username != String::new() {
        let left = format!("{uname} left!", uname = name.username);

        let _ = state.tx.send(left);
    }
    
}

#[async_recursion]
async fn verify_user(
    state : &Arc<AppState>,
    mut name: &mut UserLookup,
    r: &mut SplitStream<WebSocket>,
    s: &mut SplitSink<WebSocket, Message>
) {
    while let Some(Ok(message)) = r.next().await {
       
        if let Message::Text(uname) = message {

            println!("{}", uname);
            let joining_user = serde_json::from_str(&uname).expect("Failed to json");
            //println!("{}", name);
            get_valid_username(&state, &mut name, &joining_user, true);

            println!("{}", name.username);
            if !name.username.is_empty() {
                break;
            } else {
                let _ = s
                    .send(Message::Text(String::from("Please Register this user")))
                    .await;
                let _ = r.next().await;
                verify_user(state, name, r, s).await;
            }
        }
    }
    return;

}


fn get_valid_username(
    state: &AppState,
    string: &mut UserLookup,
    name: &UserLookup,
    session_persist: bool
){
    let user_name = state.user_name.lock().unwrap();
    let user_pass = state.user_pass.lock().unwrap();

    let user = user_name.iter().position(|e| e == &name.username);

    if !session_persist {
        if user != None  && (name.username != String::new()) && user_pass[user.unwrap()] == name.password{
            string.username = name.username.clone();
            string.password = name.password.clone();
        }
    } else {
        if user != None  && (name.username != String::new()) && digest(&user_pass[user.unwrap()]) == name.password{
            string.username = name.username.clone();
            string.password = name.password.clone();
        } 
    }
    
    
}
