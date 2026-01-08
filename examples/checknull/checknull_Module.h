#include <vector>
#define OXLIB_DEFAULT_CONFIG
#include <oxlib/oxlib.h>
#include <oxlib/result.h>
#include <oxlib/time.h>
#include <oxlib/math.h>
#include "../omnix/common.h"

#include <zeus.h>

#include "../modules/window_module.h"
#include "../modules/input_module.h"


class game_main:public module{
    public:
	controller* Controller;
	logger* Logger;
	module_manager* Modules;

    window_module* window;
    input_module* input;
	game_main(manager** Managers) :module(module_info{
		.Static = {
		.UniqueId = 1234,
		.Name = "CheckNull",
		.Version = {0, 1, 0},
		.Type = module_type::STATIC,
		.Dependencies = {
			dependency{
                32,"window_module",{0,1,0},
                dependency::dependency_type::REQUIRED,
                {
                    .init_before = false,
                    .init_after = true,
                    .loop_before = true,
                    .loop_after = false,
                    .shutdown_before = true,
                    .shutdown_after = false,
                }
            },
            dependency{
                776,"input_module",{0,1,0},
                dependency::dependency_type::REQUIRED,
                {
                    .init_before = false,
                    .init_after = true,
                    .loop_before = true,
                    .loop_after = false,
                    .shutdown_before = true,
                    .shutdown_after = false,
                }
            },
            dependency{
                1000,"graphics_module",{0,1,0},
                dependency::dependency_type::REQUIRED,
                {
                    .init_before = false,
                    .init_after = true,
                    .loop_before = true,
                    .loop_after = false,
                    .shutdown_before = true,
                    .shutdown_after = false,
                }
            }
		},
		.Path = nullptr
		}
		}, Managers) {
	}
	ox::result init() override;
	ox::result update(double dt) override;
	ox::result shutdown() override;
};



#ifndef CHESS_ENGINE_H
#define CHESS_ENGINE_H

#include <vector>

enum class PieceType { NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum class Color { NONE, WHITE, BLACK };

struct Piece {
    PieceType type;
    Color color;
    
    Piece() : type(PieceType::NONE), color(Color::NONE) {}
    Piece(PieceType t, Color c) : type(t), color(c) {}
    
    bool isEmpty() const { return type == PieceType::NONE; }
};

struct Position {
    int row, col;
    Position(int r = 0, int c = 0) : row(r), col(c) {}
    bool isValid() const { return row >= 0 && row < 8 && col >= 0 && col < 8; }
    bool operator==(const Position& p) const { return row == p.row && col == p.col; }
};

struct Move {
    Position from, to;
    PieceType promotion;
    bool isCastling;
    bool isEnPassant;
    
    Move(Position f, Position t, PieceType p = PieceType::QUEEN) 
        : from(f), to(t), promotion(p), isCastling(false), isEnPassant(false) {}
};

struct GameState {
    Piece board[8][8];
    Color turn;
    bool castlingRights[4]; // 0:WK, 1:WQ, 2:BK, 3:BQ
    Position enPassantSquare;
    int halfmoveClock;
    int fullmoveNumber;
    std::vector<Move> moveHistory;
};

class ChessEngine {
private:
    GameState state;
    
    // Yardımcı fonksiyonlar
    Color opponent(Color c) const {
        return c == Color::WHITE ? Color::BLACK : Color::WHITE;
    }
    
    bool isSquareAttacked(Position pos, Color attacker) const {
        // Piyon saldırıları
        int pawnDir = (attacker == Color::WHITE) ? -1 : 1;
        Position pawnAttacks[] = {
            Position(pos.row + pawnDir, pos.col - 1),
            Position(pos.row + pawnDir, pos.col + 1)
        };
        for (auto& p : pawnAttacks) {
            if (p.isValid()) {
                const Piece& piece = state.board[p.row][p.col];
                if (piece.type == PieceType::PAWN && piece.color == attacker)
                    return true;
            }
        }
        
        // At saldırıları
        int knightMoves[][2] = {{-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1}};
        for (auto& m : knightMoves) {
            Position p(pos.row + m[0], pos.col + m[1]);
            if (p.isValid()) {
                const Piece& piece = state.board[p.row][p.col];
                if (piece.type == PieceType::KNIGHT && piece.color == attacker)
                    return true;
            }
        }
        
        // Kale/Vezir saldırıları (yatay/dikey)
        int directions[][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        for (auto& dir : directions) {
            for (int i = 1; i < 8; i++) {
                Position p(pos.row + dir[0]*i, pos.col + dir[1]*i);
                if (!p.isValid()) break;
                const Piece& piece = state.board[p.row][p.col];
                if (!piece.isEmpty()) {
                    if (piece.color == attacker && 
                        (piece.type == PieceType::ROOK || piece.type == PieceType::QUEEN))
                        return true;
                    break;
                }
            }
        }
        
        // Fil/Vezir saldırıları (çapraz)
        int diagDirs[][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};
        for (auto& dir : diagDirs) {
            for (int i = 1; i < 8; i++) {
                Position p(pos.row + dir[0]*i, pos.col + dir[1]*i);
                if (!p.isValid()) break;
                const Piece& piece = state.board[p.row][p.col];
                if (!piece.isEmpty()) {
                    if (piece.color == attacker && 
                        (piece.type == PieceType::BISHOP || piece.type == PieceType::QUEEN))
                        return true;
                    break;
                }
            }
        }
        
        // Şah saldırıları
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) continue;
                Position p(pos.row + dr, pos.col + dc);
                if (p.isValid()) {
                    const Piece& piece = state.board[p.row][p.col];
                    if (piece.type == PieceType::KING && piece.color == attacker)
                        return true;
                }
            }
        }
        
        return false;
    }
    
    Position findKing(Color color) const {
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                const Piece& p = state.board[r][c];
                if (p.type == PieceType::KING && p.color == color)
                    return Position(r, c);
            }
        }
        return Position(-1, -1);
    }
    
    bool isInCheck(Color color) const {
        Position kingPos = findKing(color);
        return isSquareAttacked(kingPos, opponent(color));
    }
    
    bool wouldBeInCheck(const Move& move) const {
        // Geçici hamle yap
        Piece tempFrom = state.board[move.from.row][move.from.col];
        Piece tempTo = state.board[move.to.row][move.to.col];
        
        const_cast<Piece&>(state.board[move.to.row][move.to.col]) = tempFrom;
        const_cast<Piece&>(state.board[move.from.row][move.from.col]) = Piece();
        
        bool inCheck = isInCheck(tempFrom.color);
        
        // Geri al
        const_cast<Piece&>(state.board[move.from.row][move.from.col]) = tempFrom;
        const_cast<Piece&>(state.board[move.to.row][move.to.col]) = tempTo;
        
        return inCheck;
    }
    
    std::vector<Move> getPseudoLegalMoves(Position pos) const {
        std::vector<Move> moves;
        const Piece& piece = state.board[pos.row][pos.col];
        
        if (piece.isEmpty()) return moves;
        
        switch (piece.type) {
            case PieceType::PAWN: {
                int dir = (piece.color == Color::WHITE) ? -1 : 1;
                int startRow = (piece.color == Color::WHITE) ? 6 : 1;
                int promoRow = (piece.color == Color::WHITE) ? 0 : 7;
                
                // İleri hareket
                Position forward(pos.row + dir, pos.col);
                if (forward.isValid() && state.board[forward.row][forward.col].isEmpty()) {
                    if (forward.row == promoRow) {
                        moves.push_back(Move(pos, forward, PieceType::QUEEN));
                        moves.push_back(Move(pos, forward, PieceType::ROOK));
                        moves.push_back(Move(pos, forward, PieceType::BISHOP));
                        moves.push_back(Move(pos, forward, PieceType::KNIGHT));
                    } else {
                        moves.push_back(Move(pos, forward));
                    }
                    
                    // İlk hamlede 2 kare
                    if (pos.row == startRow) {
                        Position forward2(pos.row + 2*dir, pos.col);
                        if (state.board[forward2.row][forward2.col].isEmpty())
                            moves.push_back(Move(pos, forward2));
                    }
                }
                
                // Çapraz yeme
                for (int dc : {-1, 1}) {
                    Position capture(pos.row + dir, pos.col + dc);
                    if (capture.isValid()) {
                        const Piece& target = state.board[capture.row][capture.col];
                        if (!target.isEmpty() && target.color != piece.color) {
                            if (capture.row == promoRow) {
                                moves.push_back(Move(pos, capture, PieceType::QUEEN));
                                moves.push_back(Move(pos, capture, PieceType::ROOK));
                                moves.push_back(Move(pos, capture, PieceType::BISHOP));
                                moves.push_back(Move(pos, capture, PieceType::KNIGHT));
                            } else {
                                moves.push_back(Move(pos, capture));
                            }
                        }
                        
                        // En passant
                        if (state.enPassantSquare == capture) {
                            Move m(pos, capture);
                            m.isEnPassant = true;
                            moves.push_back(m);
                        }
                    }
                }
                break;
            }
            
            case PieceType::KNIGHT: {
                int knightMoves[][2] = {{-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1}};
                for (auto& m : knightMoves) {
                    Position target(pos.row + m[0], pos.col + m[1]);
                    if (target.isValid()) {
                        const Piece& targetPiece = state.board[target.row][target.col];
                        if (targetPiece.isEmpty() || targetPiece.color != piece.color)
                            moves.push_back(Move(pos, target));
                    }
                }
                break;
            }
            
            case PieceType::BISHOP: {
                int dirs[][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};
                for (auto& dir : dirs) {
                    for (int i = 1; i < 8; i++) {
                        Position target(pos.row + dir[0]*i, pos.col + dir[1]*i);
                        if (!target.isValid()) break;
                        const Piece& targetPiece = state.board[target.row][target.col];
                        if (targetPiece.isEmpty()) {
                            moves.push_back(Move(pos, target));
                        } else {
                            if (targetPiece.color != piece.color)
                                moves.push_back(Move(pos, target));
                            break;
                        }
                    }
                }
                break;
            }
            
            case PieceType::ROOK: {
                int dirs[][2] = {{-1,0},{1,0},{0,-1},{0,1}};
                for (auto& dir : dirs) {
                    for (int i = 1; i < 8; i++) {
                        Position target(pos.row + dir[0]*i, pos.col + dir[1]*i);
                        if (!target.isValid()) break;
                        const Piece& targetPiece = state.board[target.row][target.col];
                        if (targetPiece.isEmpty()) {
                            moves.push_back(Move(pos, target));
                        } else {
                            if (targetPiece.color != piece.color)
                                moves.push_back(Move(pos, target));
                            break;
                        }
                    }
                }
                break;
            }
            
            case PieceType::QUEEN: {
                int dirs[][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
                for (auto& dir : dirs) {
                    for (int i = 1; i < 8; i++) {
                        Position target(pos.row + dir[0]*i, pos.col + dir[1]*i);
                        if (!target.isValid()) break;
                        const Piece& targetPiece = state.board[target.row][target.col];
                        if (targetPiece.isEmpty()) {
                            moves.push_back(Move(pos, target));
                        } else {
                            if (targetPiece.color != piece.color)
                                moves.push_back(Move(pos, target));
                            break;
                        }
                    }
                }
                break;
            }
            
            case PieceType::KING: {
                for (int dr = -1; dr <= 1; dr++) {
                    for (int dc = -1; dc <= 1; dc++) {
                        if (dr == 0 && dc == 0) continue;
                        Position target(pos.row + dr, pos.col + dc);
                        if (target.isValid()) {
                            const Piece& targetPiece = state.board[target.row][target.col];
                            if (targetPiece.isEmpty() || targetPiece.color != piece.color)
                                moves.push_back(Move(pos, target));
                        }
                    }
                }
                
                // Rok
                int row = (piece.color == Color::WHITE) ? 7 : 0;
                if (pos.row == row && pos.col == 4 && !isInCheck(piece.color)) {
                    // Kısa rok
                    int kIdx = (piece.color == Color::WHITE) ? 0 : 2;
                    if (state.castlingRights[kIdx]) {
                        if (state.board[row][5].isEmpty() && state.board[row][6].isEmpty()) {
                            if (!isSquareAttacked(Position(row, 5), opponent(piece.color)) &&
                                !isSquareAttacked(Position(row, 6), opponent(piece.color))) {
                                Move m(pos, Position(row, 6));
                                m.isCastling = true;
                                moves.push_back(m);
                            }
                        }
                    }
                    
                    // Uzun rok
                    int qIdx = (piece.color == Color::WHITE) ? 1 : 3;
                    if (state.castlingRights[qIdx]) {
                        if (state.board[row][3].isEmpty() && state.board[row][2].isEmpty() && 
                            state.board[row][1].isEmpty()) {
                            if (!isSquareAttacked(Position(row, 3), opponent(piece.color)) &&
                                !isSquareAttacked(Position(row, 2), opponent(piece.color))) {
                                Move m(pos, Position(row, 2));
                                m.isCastling = true;
                                moves.push_back(m);
                            }
                        }
                    }
                }
                break;
            }
            
            default:
                break;
        }
        
        return moves;
    }

public:
    ChessEngine() {
        reset();
    }
    
    void reset() {
        // Tahtayı başlangıç pozisyonuna kur
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                state.board[i][j] = Piece();
        
        // Siyah taşlar
        state.board[0][0] = Piece(PieceType::ROOK, Color::BLACK);
        state.board[0][1] = Piece(PieceType::KNIGHT, Color::BLACK);
        state.board[0][2] = Piece(PieceType::BISHOP, Color::BLACK);
        state.board[0][3] = Piece(PieceType::QUEEN, Color::BLACK);
        state.board[0][4] = Piece(PieceType::KING, Color::BLACK);
        state.board[0][5] = Piece(PieceType::BISHOP, Color::BLACK);
        state.board[0][6] = Piece(PieceType::KNIGHT, Color::BLACK);
        state.board[0][7] = Piece(PieceType::ROOK, Color::BLACK);
        for (int i = 0; i < 8; i++)
            state.board[1][i] = Piece(PieceType::PAWN, Color::BLACK);
        
        // Beyaz taşlar
        for (int i = 0; i < 8; i++)
            state.board[6][i] = Piece(PieceType::PAWN, Color::WHITE);
        state.board[7][0] = Piece(PieceType::ROOK, Color::WHITE);
        state.board[7][1] = Piece(PieceType::KNIGHT, Color::WHITE);
        state.board[7][2] = Piece(PieceType::BISHOP, Color::WHITE);
        state.board[7][3] = Piece(PieceType::QUEEN, Color::WHITE);
        state.board[7][4] = Piece(PieceType::KING, Color::WHITE);
        state.board[7][5] = Piece(PieceType::BISHOP, Color::WHITE);
        state.board[7][6] = Piece(PieceType::KNIGHT, Color::WHITE);
        state.board[7][7] = Piece(PieceType::ROOK, Color::WHITE);
        
        state.turn = Color::WHITE;
        for (int i = 0; i < 4; i++)
            state.castlingRights[i] = true;
        state.enPassantSquare = Position(-1, -1);
        state.halfmoveClock = 0;
        state.fullmoveNumber = 1;
        state.moveHistory.clear();
    }
    
    // Tüm yasal hamleleri al
    std::vector<Move> getLegalMoves(Position pos) const {
        std::vector<Move> pseudoMoves = getPseudoLegalMoves(pos);
        std::vector<Move> legalMoves;
        
        for (const Move& move : pseudoMoves) {
            if (!wouldBeInCheck(move))
                legalMoves.push_back(move);
        }
        
        return legalMoves;
    }
    
    // Tüm yasal hamleleri al (tüm taşlar)
    std::vector<Move> getAllLegalMoves() const {
        std::vector<Move> allMoves;
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                const Piece& piece = state.board[r][c];
                if (!piece.isEmpty() && piece.color == state.turn) {
                    auto moves = getLegalMoves(Position(r, c));
                    allMoves.insert(allMoves.end(), moves.begin(), moves.end());
                }
            }
        }
        return allMoves;
    }
    
    // Hamle yap
    bool makeMove(const Move& move) {
        const Piece& piece = state.board[move.from.row][move.from.col];
        
        if (piece.isEmpty() || piece.color != state.turn){
            return false;
        }
        
        // Yasal hamle mi kontrol et
        auto legalMoves = getLegalMoves(move.from);
        bool isLegal = false;
        for (const Move& m : legalMoves) {
            if (m.from == move.from && m.to == move.to) {
                isLegal = true;
                break;
            }
        }
        
        if (!isLegal) return false;
        
        // Hamleyi kaydet
        state.moveHistory.push_back(move);
        
        // En passant kontrolü
        Position oldEnPassant = state.enPassantSquare;
        state.enPassantSquare = Position(-1, -1);
        
        // Hamleyi uygula
        Piece movingPiece = state.board[move.from.row][move.from.col];
        state.board[move.to.row][move.to.col] = movingPiece;
        state.board[move.from.row][move.from.col] = Piece();
        
        // Özel durumlar
        if (move.isCastling) {
            // Kaleyi taşı
            if (move.to.col == 6) { // Kısa rok
                state.board[move.to.row][5] = state.board[move.to.row][7];
                state.board[move.to.row][7] = Piece();
            } else { // Uzun rok
                state.board[move.to.row][3] = state.board[move.to.row][0];
                state.board[move.to.row][0] = Piece();
            }
        }
        
        if (move.isEnPassant) {
            int captureRow = (movingPiece.color == Color::WHITE) ? move.to.row + 1 : move.to.row - 1;
            state.board[captureRow][move.to.col] = Piece();
        }
        
        // Piyon terfisi
        if (movingPiece.type == PieceType::PAWN) {
            int promoRow = (movingPiece.color == Color::WHITE) ? 0 : 7;
            if (move.to.row == promoRow) {
                state.board[move.to.row][move.to.col].type = move.promotion;
            }
            
            // Piyon 2 kare ilerledi mi?
            if (abs(move.from.row - move.to.row) == 2) {
                state.enPassantSquare = Position((move.from.row + move.to.row) / 2, move.to.col);
            }
        }
        
        // Rok haklarını güncelle
        if (movingPiece.type == PieceType::KING) {
            if (movingPiece.color == Color::WHITE) {
                state.castlingRights[0] = false;
                state.castlingRights[1] = false;
            } else {
                state.castlingRights[2] = false;
                state.castlingRights[3] = false;
            }
        }
        
        if (movingPiece.type == PieceType::ROOK) {
            if (move.from.row == 7 && move.from.col == 0) state.castlingRights[1] = false;
            if (move.from.row == 7 && move.from.col == 7) state.castlingRights[0] = false;
            if (move.from.row == 0 && move.from.col == 0) state.castlingRights[3] = false;
            if (move.from.row == 0 && move.from.col == 7) state.castlingRights[2] = false;
        }
        
        // Sırayı değiştir
        state.turn = opponent(state.turn);
        if (state.turn == Color::WHITE)
            state.fullmoveNumber++;
        
        return true;
    }
    
    // Oyun durumu
    bool isCheckmate() const {
        return isInCheck(state.turn) && getAllLegalMoves().empty();
    }
    
    bool isStalemate() const {
        return !isInCheck(state.turn) && getAllLegalMoves().empty();
    }
    
    bool isCheck() const {
        return isInCheck(state.turn);
    }
    
    // Getter'lar
    const Piece& getPiece(int row, int col) const {
        return state.board[row][col];
    }
    
    Color getTurn() const {
        return state.turn;
    }
    
    const GameState& getState() const {
        return state;
    }
};
#endif // CHESS_ENGINE_H


struct checknull{
    ChessEngine Engine;
    zs::spritebatch* batch;

    static constexpr const int BoardSize = 142;
    static constexpr const int PieceSize = 16;

    zs::sprite* Board;

    std::vector<std::array<zs::texture::batching::UV, 4>> sheet = 
    zs::texture::batching::batching::generate_sprite_sheet_uvs(1024, 1024, 16, 16);

    std::array<zs::texture::batching::UV, 4> Tex(PieceType type,Color col){
        switch (col) {

        case Color::NONE:{
            return {};
        }
        case Color::WHITE:{
            switch (type) {
            case PieceType::NONE: return {};
            case PieceType::PAWN: return sheet[6];
            case PieceType::KNIGHT: return sheet[7];
            case PieceType::BISHOP: return sheet[9];
            case PieceType::ROOK: return sheet[8];
            case PieceType::QUEEN: return sheet[10];
            case PieceType::KING: return sheet[11];
              break;
            }
        }
        case Color::BLACK:{
            switch (type) {
            case PieceType::NONE: return {};
            case PieceType::PAWN: return sheet[0];
            case PieceType::KNIGHT: return sheet[1];
            case PieceType::BISHOP: return sheet[3];
            case PieceType::ROOK: return sheet[2];
            case PieceType::QUEEN: return sheet[4];
            case PieceType::KING: return sheet[5];
              break;
            }
        }
          break;
        }
        return {};
    }

    struct PieceLogic{
        int batch_loc;
        int x,y;
        ox::vec2f defaults;
    };

    PieceLogic arr[8][8];
    ox::vec2f origin; 
    void init(zs::spritebatch* batch){
        this->batch = batch;
        Engine.reset();

        origin = {-(BoardSize/2.0f)+7+PieceSize/2.0f,(BoardSize/2.0f)-7-PieceSize/2.0f};

        for (int y = 0; y<8; y++) {
            for (int x = 0; x<8; x++) {
                auto piece = Engine.getPiece(y, x);
                auto spr = batch->add_sprite();
                spr->scale = {PieceSize,PieceSize};
                if(!piece.isEmpty()){
                    spr->QuadTXCoords = Tex(piece.type,piece.color);
                }else{
                    spr->textureID = ZS_IGNORE_TXID;
                    spr->color = {1,1,1,0};
                }
                spr->position = origin + ox::vec2f{x*16.0f,-y*16.0f};
                arr[y][x] = {static_cast<int>(batch->cursor-1),x,y,{spr->position.x(),spr->position.y()}};
            }
        }
        
        Board = batch->add_sprite();
        Board->textureID = 1;
        Board->z_index = -10000;
        Board->scale = {BoardSize,BoardSize};
    }

    void update(float dt){
        for (int y = 0; y<8; y++) {
            for (int x = 0; x<8; x++) {
                auto piece = Engine.getPiece(y, x);
                auto spr = &batch->sprites[arr[y][x].batch_loc];
                spr->scale = {PieceSize,PieceSize};
                if(!piece.isEmpty()){
                    spr->textureID = 0;
                    spr->QuadTXCoords = Tex(piece.type,piece.color);
                    spr->color = {1,1,1,1};
                }else{
                    spr->textureID = ZS_IGNORE_TXID;
                    spr->color = {1,1,1,0};
                }
                spr->position = origin + ox::vec2f{x*16.0f,-y*16.0f};
                batch->mark_dirty(*spr);
            }
        }
        batch->mark_dirty(*Board);
    }

    ox::vec2i holding;
    ox::vec2f drag_offset;
    ox::vec2f old_pos;
    bool dragging = false;
    void update_drag(float dt,zs::ortho_camera cam,zs::world world,int w,int h,input_module* input){
        auto mouse_screen = ox::vec2f{
            (float) input->GetMousePos().x(),
            (float) input->GetMousePos().y()
        };

        auto mouse_world = world.screen_to_world(mouse_screen, cam, w , h);
        
        auto moves = Engine.getLegalMoves({holding.y(),holding.x()});
        for(auto& move:moves){
            auto pos = arr[move.to.row][move.to.col];
            zs::dbg2d::dot(ox::vec2f::revert_y(world.world_to_screen(ox::vec3f{pos.defaults}, cam, w, h), h), 5, {1,0,0,1});
        }

        if(dragging){
            if (input->IsMouseDown(0)) {
                auto spr2 = &batch->sprites[arr[holding.y()][holding.x()].batch_loc];
                auto old = spr2->position;
                spr2->position = mouse_world + ox::vec2f{-drag_offset.x(),-drag_offset.y()};
                if(!ox::vec2f::equals(spr2->position, old)){
                    batch->mark_dirty(*spr2);
                }
                
                zs::dbg2d::dot({mouse_screen.x(),h-mouse_screen.y()}, 5, {1,0,0,1});
            } 
            else {
                Move move{{},{}};
                move.from = {holding.y(),holding.x()};
                auto pos = arr[holding.y()][holding.x()].defaults;


                for (int y = 0; y<8; y++) {
                    for (int x = 0; x<8; x++) {
                        if(x==holding.x()&&y==holding.y()) continue;
                        auto piece = Engine.getPiece(y, x);
                        auto spr = &batch->sprites[arr[y][x].batch_loc];
                        if(ox::vec2f::pointInRect(mouse_world, spr->position, spr->scale)){
                            move.to = {y,x};
                        }
                    }
                }

                Engine.makeMove(move);
                update(dt);
                dragging = false;
            }
        }else{
            for (int y = 0; y<8; y++) {
                for (int x = 0; x<8; x++) {
                    auto piece = Engine.getPiece(y, x);
                    auto spr = &batch->sprites[arr[y][x].batch_loc];
                    bool is_hovered = ox::vec2f::pointInRect(mouse_world, spr->position, spr->scale);

                    if(!dragging){
                        if(is_hovered){
                            if(input->IsMouseJustDown(0)){
                                old_pos = spr->position;
                                dragging = true;
                                holding.x() = x;
                                holding.y() = y;
                                drag_offset = mouse_world - spr->position;
                            }
                        }else{
                            if(input->IsMouseJustDown(0)){
                                auto moves = Engine.getLegalMoves({holding.y(),holding.x()});
                                for(auto& move:moves){
                                    auto pos = arr[move.to.row][move.to.col];
                                    if(ox::vec2f::pointInRect(mouse_world, pos.defaults, {16,16})){
                                        Engine.makeMove(move);
                                        update(dt);
                                        holding = {0,0};
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if(Engine.isCheckmate()){
            update(dt);
            Engine.reset();
        }
        if(Engine.isStalemate()){
            update(dt);
            Engine.reset();
        }
    }
};
struct scene{
    zs::ortho_camera Camera;
    zs::spritebatch Batch;
};
