#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <memory>
#include <boost/date_time.hpp>

class Piece;
class State;
class Player;

namespace pt = boost::posix_time;

typedef std::shared_ptr<Piece> PiecePtr;
typedef std::shared_ptr<State> StatePtr;
typedef std::shared_ptr<Player> PlayerPtr;

#endif //TYPEDEFS_H
