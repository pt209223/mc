#ifndef INVADERS_GTP_H_
#define INVADERS_GTP_H_

#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include <board.h>
#include <config.h>
#include <vertex.h>

class Gtp {
  public:
    // Definiujemy nazwe komendy oraz funkcje ja wykonujaca 
    class Command {
      public:
        Command(Gtp &g, const std::string &name) 
          : gtp_(g), name_(name) { gtp_.add_command(*this); }
        virtual ~Command(void) { }
        
        virtual Gtp &gtp(void) { return gtp_; }
        virtual const std::string &name(void) const { return name_; }
        virtual std::string execute(void) = 0;

      private:
        Gtp &gtp_;          //<- referencja do gtp
        std::string name_;  //<- nazwa komendy
    }; 
    
    Gtp(std::istream &i, std::ostream &o) : in_(i), out_(o) { }
    ~Gtp(void) { }
  
    // Dodanie komendy
    void add_command(Command &cmd) 
    {
      commands_.push_back(&cmd);
    }

    // odczytywanie przychodzacych komend w petli 
    void loop(void)
    {
      std::string name;

      while (true) {
        getline();
        std::string reply = "? command not found";

        if (!get_param(name)) continue; // pusta linia

        for (size_t i = 0; i < commands_.size(); ++i)
          if (commands_[i]->name() == name) {
            reply = commands_[i]->execute();
            break;
          }

        out_ << reply << "\n\n";
        out_.flush();
      }
    }

    // Bierzemy paramert na param (jak trzeba wczytuje linie)
    bool get_param(std::string &param, bool more = false)
    {
      if (!more) return (line_ >> param);

      while (!(line_ >> param))
        if (!getline()) return false;

      return true;
    }

    // Wczytanie linni
    bool getline(void)
    {
      std::string line;
      line_.str("");
      line_.clear();

      if (std::getline(in_, line)) {
        line_ << line;
        return true;
      }

      return false;
    }

    friend class GtpCommands;

  private:
    std::vector<Command *> commands_;    //<- tablica komend
    std::istream &in_;                   //<- strumien wejsciowy
    std::ostream &out_;                  //<- strumien wyjsciowy
    std::stringstream line_;             //<- parametry
};

// Wybor postawienia pionka wg wskazanego silnika
template <typename engine_t>
class GtpGenplace : public Gtp::Command {
  public:
    GtpGenplace(Gtp &gtp, engine_t &engine, Board &board) 
      : Gtp::Command(gtp, "genplace"), engine_(engine), board_(board) { }

    std::string execute(void)
    {
      std::string param;
      
      if (!gtp().get_param(param)) return "? missing player";

      Player pl = Player::scan(param);
      if (pl == Player::any()) return "? wrong player";

      Move mv = engine_.genplace(pl);
      board_.move(mv);
      
      switch (mv.type()) {
        case Move::Resign:
          return "= resign";
        case Move::Pass:
          return "= pass";
        case Move::Place:
          return "= " + mv.to().to_string();
        default:
          abort();
      }

      abort();
    }

  private:
    engine_t &engine_;  //<- referencja do silnika
    Board &board_;      //<- referencja do planszy
};

// Wybor ruchu pionka wg wskazanego silnika
template <typename engine_t>
class GtpGenmove : public Gtp::Command {
  public:
    GtpGenmove(Gtp &gtp, engine_t &engine, Board &board) 
      : Gtp::Command(gtp, "genmove"), engine_(engine), board_(board) { }

    std::string execute(void)
    {
      std::string param;
      if (!gtp().get_param(param)) return "? missing player";
      
      Player pl = Player::scan(param);
      if (pl == Player::any()) return "? wrong player";

      Move mv = engine_.genmove(pl);
      board_.move(mv);

      switch (mv.type()) {
        case Move::Resign: 
          return "= resign";
        case Move::Pass:   
          return "= pass";
        case Move::Mov:   
          return "= " + mv.from().to_string() + " " + mv.to().to_string() + " " + mv.block().to_string();
        default:
          abort();
      }

      abort();
    }
  private:
    engine_t &engine_;  //<- referencja do silnika
    Board &board_;      //<- referencja do planszy
};

// Umieszczenie pionka
class GtpPlace : public Gtp::Command {
  public:
    GtpPlace(Gtp &gtp, Board &board) 
      : Gtp::Command(gtp, "place"), board_(board) { }

    std::string execute(void)
    {
      std::string player, coord;

      if (!gtp().get_param(player)) return "? missing player";
      if (!gtp().get_param(coord)) return "? missing coordinates";

      Player pl = Player::scan(player);
      Vertex vx = Vertex::scan(coord);
      Move mv = Move(pl, Move::Pass);

      if (pl == Player::any()) return "? wrong player";

      if (vx == Vertex::any()) { 
        if (coord != "pass" && coord != "resign") return "? wrong coordinates";
      } else {
        mv = Move(pl, vx);
        if (!board_.is_legal_move(mv))
          return "? illegal move: " + pl.to_string() + " " + vx.to_string();
      }

      board_.move(mv);
      return "= ";
    }
  private:
    Board &board_; //<- referencja do planszy
};

// Ruch pionkiem
class GtpMove : public Gtp::Command {
  public:
    GtpMove(Gtp &gtp, Board &board) 
      : Gtp::Command(gtp, "move"), board_(board) { }

    std::string execute(void)
    {
      std::string player, from, to, block;

      if (!gtp().get_param(player)) return "? missing player";
      if (!gtp().get_param(from)) return "? missing source coordinates";

      if (from == "pass" || from == "resign") {
        Player pl = Player::scan(player);
        if (pl == Player::any()) return "? wrong player";

        board_.move(Move(pl, Move::Pass));
        return "= ";
      }

      if (!gtp().get_param(to)) return "? missing destination coordinates";
      if (!gtp().get_param(block)) return "? missing blocked coordinates";
      
      Player pl = Player::scan(player);
      Vertex vf = Vertex::scan(from),
             vt = Vertex::scan(to),
             vb = Vertex::scan(block);

      if (pl == Player::any()) return "? wrong player";
      if (vf == Vertex::any()) return "? wrong source";
      if (vt == Vertex::any()) return "? wrong destination";
      if (vb == Vertex::any()) return "? wrong block";
      if (!board_.is_legal_move(Move(pl, vf, vt, vb))) 
        return "? illegal move: " + pl.to_string() + " " + vf.to_string() + " " + vt.to_string() + " " + vb.to_string();

      board_.move(Move(pl, vf, vt, vb));
      return "= ";
    }
  private:
    Board &board_; //<- referencja do planszy
};

// Ustawienie pozostalego czasu
class GtpTimeLeft : public Gtp::Command {
  public:
    GtpTimeLeft(Gtp &gtp, uint &time_left) 
      : Gtp::Command(gtp, "time_left"), time_left_(time_left) { }

    std::string execute(void)
    {
      std::string param;
      if (!gtp().get_param(param)) return "? missing miliseconds";

      time_left_ = atoi(param.c_str());
      return "= ";
    }
  private:
    //static const uint max_time_left_ = 60*1000; // 1min
    uint &time_left_;  //<- referencja do zmiennej
};

// Ustawienie planszy
class GtpSetBoard : public Gtp::Command {
  public:
    GtpSetBoard(Gtp &gtp, Board &board) 
      : Gtp::Command(gtp, "setboard"), board_(board) { }

    std::string execute(void)
    {
      std::string bsize, psize;
      if (!gtp().get_param(bsize, true)) return "? missing board size";
      if (!gtp().get_param(psize, true)) return "? missing pawns per player";

      uint board_size = atoi(bsize.c_str()),
           pawns_per_player = atoi(psize.c_str());

      if (board_size != BoardSize) return "? wrong board size";
      if (pawns_per_player != PawnSize) return "? wrong number pawns per player";

      board_.clear();
      std::string line;

      for (uint i = 0; i < BoardSize; ++i) {
        if (!gtp().get_param(line, true)) return "? missing board description";

        if (line.length() != BoardSize)
          return "? wrong board description";

        for (uint j = 0; j < BoardSize; ++j) {
          if ('.' != line[j] && '#' != line[j])
            return "? wrong board description";

          board_.set_field(Vertex(i,j), line[j]=='.' ? Field::empty():Field::block());
        }
      }

      return "= ";
    }
  private:
    Board &board_;
};

// Pokazanie planszy
class GtpShowBoard : public Gtp::Command {
  public:
    GtpShowBoard(Gtp &gtp, Board &board)
      : Gtp::Command(gtp, "showboard"), board_(board) { }

    std::string execute(void)
    {
      return "= " + board_.to_string();
    }

  private:
    Board &board_;
};

// Wersja
class GtpVersion : public Gtp::Command {
  public: 
    GtpVersion(Gtp &gtp, const std::string &vers) 
      : Gtp::Command(gtp, "version"), version_(vers) { }

    std::string execute(void)
    {
      return "= " + version_;
    }
  private:
    const std::string version_;
};

// Autor
class GtpAuthor : public Gtp::Command {
  public:
    GtpAuthor(Gtp &gtp, const std::string &author) 
      : Gtp::Command(gtp, "author"), author_(author) { }

    std::string execute(void)
    {
      return "= " + author_;
    }
  private:
    const std::string author_;
}; 

// Nazwa
class GtpName : public Gtp::Command {
  public:
    GtpName(Gtp &gtp, const std::string &name)
      : Gtp::Command(gtp, "name"), name_(name) { }

    std::string execute(void)
    {
      return "= " + name_;
    }
  private:
    const std::string name_;
};

// Lista dostepnych komend
class GtpCommands : public Gtp::Command {
  public:
    GtpCommands(Gtp &gtp)
      : Gtp::Command(gtp, "commands") { }

    std::string execute(void)
    {
      std::string out = "=";
      for (uint i = 0; i < gtp().commands_.size(); ++i) 
        out += " " + gtp().commands_[i]->name();
      return out;
    }
};

// Lista dozwolonych ruchow
class GtpLegalMoves : public Gtp::Command {
  public:
    GtpLegalMoves(Gtp &gtp, Board &board)
      : Gtp::Command(gtp, "legal_moves"), board_(board) { }

    std::string execute(void)
    {
      std::string param;
      if (!gtp().get_param(param)) return "? missing player";

      Player pl = Player::scan(param);
      if (Player::any() == pl) return "? wrong player";

      Move ms[320];
      uint len = board_.get_legal_moves(ms, pl);

      std::string out = "= ";
      
      for (uint i = 0; i < len; ++i) 
        out += (" " + ms[i].from().to_string());

      return out;
    }
  private:
    Board &board_;
};


#endif

