use actix_web::{web, App, HttpServer, HttpResponse, Responder};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::sync::Mutex;

#[derive(Deserialize)]
struct AuthRequest {
    response_type: String,
    client_id: String,
    redirect_uri: String,
    state: Option<String>,
    username: String,
    password: String,
}

#[derive(Deserialize)]
struct TokenRequest {
    grant_type: String,
    code: String,
    client_id: String,
    client_secret: String,
    redirect_uri: String,
}

#[derive(Serialize)]
struct TokenResponse {
    access_token: String,
    token_type: String,
    expires_in: u64,
}

struct AppState {
    codes: Mutex<HashMap<String, String>>, // auth_code -> client_id
    users: HashMap<String, String>,        // username -> password
}

async fn authorize(query: web::Query<AuthRequest>, data: web::Data<AppState>) -> impl Responder {
    // Authenticate user
    if let Some(stored_pw) = data.users.get(&query.username) {
        if stored_pw == &query.password {
            let code = "authcode123"; // Normally generated securely
            data.codes.lock().unwrap().insert(code.to_string(), query.client_id.clone());

            let redirect = format!(
                "{}?code={}&state={}",
                query.redirect_uri,
                code,
                query.state.clone().unwrap_or_default()
            );
            return HttpResponse::Found().header("Location", redirect).finish();
        }
    }
    HttpResponse::Unauthorized().body("Invalid username or password")
}

async fn token(form: web::Form<TokenRequest>, data: web::Data<AppState>) -> impl Responder {
    let codes = data.codes.lock().unwrap();
    if codes.get(&form.code) == Some(&form.client_id) {
        let response = TokenResponse {
            access_token: "access_token_abc".to_string(),
            token_type: "Bearer".to_string(),
            expires_in: 3600,
        };
        HttpResponse::Ok().json(response)
    } else {
        HttpResponse::BadRequest().body("Invalid code or client_id")
    }
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    let state = web::Data::new(AppState {
        codes: Mutex::new(HashMap::new()),
        users: HashMap::from([
            ("alice".to_string(), "password123".to_string()),
            ("bob".to_string(), "secure456".to_string()),
        ]),
    });

    HttpServer::new(move || {
        App::new()
            .app_data(state.clone())
            .route("/authorize", web::get().to(authorize))
            .route("/token", web::post().to(token))
    })
    .bind("127.0.0.1:8080")?
    .run()
    .await
}
