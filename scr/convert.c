#include "../inc/convert.h"

#include <string.h>
#include <stdint.h>

int last, first;

int convertHeader( headerChunk* sample, const uint8_t* index )
{
  uint32_t offset = 0;

  memcpy( sample->header.data, &index[offset], HEADER_BYTES );
  for ( last = HEADER_BYTES - 1, first = 0; last > ( HEADER_BYTES - 1 ) / 2; --last, ++first )
  {
    sample->header.data[first] ^= sample->header.data[last];
    sample->header.data[last] ^= sample->header.data[first];
    sample->header.data[first] ^= sample->header.data[last];
  }

  memcpy( sample->size.data, &index[offset + HEADER_BYTES], HEADER_BYTES );
  for ( last = HEADER_BYTES - 1, first = 0; last > ( HEADER_BYTES - 1 ) / 2; --last, ++first )
  {
    sample->size.data[first] ^= sample->size.data[last];
    sample->size.data[last] ^= sample->size.data[first];
    sample->size.data[first] ^= sample->size.data[last];
  }

  memcpy( sample->field.data, &index[offset + 2 * HEADER_BYTES], HEADER_SIZE );
  for ( last = HEADER_SIZE - 1, first = 0; last > ( HEADER_SIZE - 1 ) / 2; --last, ++first )
  {
    sample->field.data[first] ^= sample->field.data[last];
    sample->field.data[last] ^= sample->field.data[first];
    sample->field.data[first] ^= sample->field.data[last];
  }

  return ( HEADER_SIZE + 2 * HEADER_BYTES );
}
uint32_t noteOn = 0;

uint32_t noteOff = 0;

int convertEvent( trackChunk* sample, const uint8_t* index, uint32_t* deltaTime, uint32_t* events )
{
  uint32_t offset = 0;
  uint32_t _deltaTime = 0;
  while ( index[offset] & 0b10000000 )
  {
    _deltaTime = ( _deltaTime << 7 ) + ( index[offset++] & 0b01111111 );
  }
  _deltaTime = ( _deltaTime << 7 ) + ( index[offset++] & 0b01111111 );
  //Update delta Time
  ( *deltaTime ) += _deltaTime;
  uint8_t event = EVENT_MASK & index[offset];

  switch ( event )
  {
    case NOTE_ON:
    {
      printf( "\tNote On \n" );
      ( *events )++;
      command com = { 1, index[offset] & 0x0F, _deltaTime, index[offset + 1] };

      if ( index[offset + 2] == 0x00 )
      {
        //Doch Note aus
        com.command = 0;
      }
      printCommand( com );
      ( *deltaTime ) = 0;

      offset += 3;
      noteOn++;
      break;
    }
    case NOTE_OFF:
    {
      printf( "\tNote Off \n" );
      ( *events )++;
      command com = { 0, index[offset] & 0x0F, _deltaTime, index[offset + 1] };
      printCommand( com );
      offset += 3;
      noteOff++;
      break;
    }
    case POLYPHONIC_PRESSURE:
    {
      printf( "\tPOLYPHONIC PRESSURE \n" );
      offset += 3;
      break;
    }
    case CONTROL_CHANGE:
    {
      printf( "\tCONTROL CHANGE \n" );
      offset += 3;
      break;
    }
    case PROGRAMM_CHANGE:
    {
      printf( "\tPROGRAMM CHANGE \n" );
      offset += 2;
      break;
    }
    case CHANNEL_PRESSURE:
    {
      printf( "\tCHANNEL PRESSURE \n" );
      offset += 2;
      break;
    }
    case PITCH_BENDING:
    {
      printf( "\tPITCH BENDING \n" );
      offset += 3;
      break;
    }
    default:
    {
      if ( ( index[offset] == 0xFF ) )
      {
        printf( "\tMeta \n" );

        switch ( index[offset + 1] )
        {
          case CHANNEL_SCOPE:
          {
            offset += 4;
            break;
          }
          case TRACK_END:
          {
            printf( "\tTrack End \n" );
            offset += 3;
            break;
          }
          case TEMPO_CHANGE:
          {
            offset += 6;
            break;
          }
          case SMPTE_OFFSET:
          {
            offset += 7;
            break;
          }
          case TIME_SIGNATURE:
          {
            offset += 7;
            break;
          }
          case KEY_SIGNATURE:
          {
            offset += 5;
            break;
          }
          default:
          {
            printf( "\tVariable Laenge \n" );
            offset += 2;
            offset += ( (uint8_t) index[offset] + 1 );
          }
        }
      }
      else
      {
        printf( "\tError \n" );
      }
    }
  }
  return offset;
}

int convertTrack( trackChunk* sample, const uint8_t* index )
{
  uint32_t offset = 0;
  size trackSize;
  memcpy( sample->chunk.data, &index[offset], TRACK_BYTES );
  for ( last = TRACK_BYTES - 1, first = 0; last > ( TRACK_BYTES - 1 ) / 2; --last, ++first )
  {
    sample->chunk.data[first] ^= sample->chunk.data[last];
    sample->chunk.data[last]  ^= sample->chunk.data[first];
    sample->chunk.data[first] ^= sample->chunk.data[last];
  }

  memcpy( trackSize.data, &index[offset + TRACK_BYTES], TRACK_BYTES );
  for ( last = TRACK_BYTES - 1, first = 0; last > ( TRACK_BYTES - 1 ) / 2; --last, ++first )
  {
    trackSize.data[first] ^= trackSize.data[last];
    trackSize.data[last]  ^= trackSize.data[first];
    trackSize.data[first] ^= trackSize.data[last];
  }

  uint32_t eventCounter = 0;
  uint32_t t_deltaTime = 0;
  uint32_t t_index = 0;

  while ( trackSize.size > t_index )
  {
    t_index += convertEvent( sample, &index[offset + 2 * TRACK_BYTES + t_index], &t_deltaTime, &eventCounter );
  }
  if ( eventCounter == 0 )
  {
    printEmpty();
  }

  sample->size.size = eventCounter;

  return ( trackSize.size );
}

int convertMidi( int argc, char **argv, headerChunk* headerSample )
{
  unsigned char verbose = 0;
  FILE* pFile;
  long lSize;
  uint8_t* buffer;
  size_t result;
  int i;
  midiTracks* tracks;
  tracks = (midiTracks*) malloc( sizeof(midiTracks) );
  tracks->numTracks = 0;

  /*
   * Wuenscht der Nutzer die Ausgabe der kompletten Datei ASCII-kodiert,
   * so hat er den Verbose Mode mit dem Parameter -v aktiviert.
   */
  if ( argc > 1 && argv[1][1] == 'v' )
  {
    verbose++;
    printf( "Verbose aktiviert!\n" );
  }

  if ( ( pFile = fopen( MIDI_FILE, "rb" ) ) == 0 )
  {
    fprintf( stderr, "\nDas Lesen der MIDI-Datei ist fehlgeschlagen\n" );
    exit( 1 );
  }

  /*
   * Der Dateizeiger wird mit dem Aufruf von fseek() ans Ende der Datei gesetzt
   * und sollte somit fuer das spaetere einlesen wieder an den Anfang gesetzt werden.
   */
  fseek( pFile, 0, SEEK_END );
  lSize = ftell( pFile );
  rewind( pFile );

  // Speicher alloziieren
  if ( ( buffer = (uint8_t*) malloc( sizeof(uint8_t) * lSize ) ) == 0 )
  {
    fprintf( stderr, "\nDie Speicheralloziierung ist fehlgeschlagen" );
    exit( 2 );
  }

  // Datei in den Speicher kopieren
  if ( ( result = fread( buffer, 1, lSize, pFile ) ) != lSize )
  {
    fprintf( stderr, "\nDer Kopiervorgang ist fehlgeschlagen" );
    exit( 2 );
  }

  printf( "Ausgabe der Datei %s", MIDI_FILE );
  for ( i = 0; i < lSize; )
  {
    static unsigned char trkNmbr = 1;
    printf( "%x", buffer[i] );

    if ( memcmp( &buffer[i], HEADER_CHUNK, HEADER_BYTES ) == 0 )
    {
      //headerChunk sample;
      i += convertHeader( headerSample, &buffer[i] );

      printf( "\nHEADER:\tKennung:%X Size:%i enthaelt %s, %i Tracks und %i Delta Time-ticks pro Viertelnote",
          headerSample->header.header, headerSample->size.size, HEADER_MSG[headerSample->field.field[2]],
          headerSample->field.field[1], headerSample->field.field[0] );
    }
    else if ( memcmp( &buffer[i], TRACK_CHUNK, TRACK_BYTES ) == 0 )
    {
      tracks->numTracks = tracks->numTracks + 1;
      tracks->tracks = (trackChunk*) realloc( tracks->tracks, tracks->numTracks * sizeof(trackChunk) );

      printCommadBegin( tracks->numTracks );
      printBeginCommand();

      i += convertTrack( & ( tracks->tracks[tracks->numTracks - 1] ), &buffer[i] ) + 2 * TRACK_BYTES;

      printEndCommand();
    }
    else
    {
      exit( 0 );
    }
    if ( verbose == 1 )
    {
      buffer[i] == '\n' ? printf( "\n" ) : printf( "%X", (unsigned int) buffer[i] );
    }

  }

  printCommandList( tracks->numTracks );
  printChunkList( tracks );
  printMidi( tracks->numTracks, headerSample->field.field[0] );
  printf( "On: %d Off: %d", noteOn, noteOff );
  /*
   * Stefan Oppel - MATLAB example:
   * if ~(Dateiname==0)
   */

  printf( "\n" );
  fclose( pFile );
  free( buffer );
  return ( 0 );
}

void printPragmaBegin()
{
  FILE* fd;
  fd = fopen( MIDI_FILE_OUT, "a+" );

  if ( fd != NULL )
  {
    fprintf( fd, "#ifndef FOOBAR_H\n#define FOOBAR_H\n" );
    fclose( fd );
  }
}

void printPragmaEnd()
{
  FILE* fd;
  fd = fopen( MIDI_FILE_OUT, "a+" );

  if ( fd != NULL )
  {
    fprintf( fd, "#endif //FOOBAR_H\n" );
    fclose( fd );
  }
}

void printInclude()
{
  FILE* fd;
  fd = fopen( MIDI_FILE_OUT, "a+" );

  if ( fd != NULL )
  {
    fprintf( fd, "#include \"../inc/definesMidi.h\"\n\n" );
    fclose( fd );
  }
}

void printCommadBegin( uint8_t trackNumber )
{
  FILE* fd;
  fd = fopen( MIDI_FILE_OUT, "a+" );

  if ( fd != NULL )
  {
    fprintf( fd, "command track%d[] = ", trackNumber );
    fclose( fd );
  }
}

void printCommand( command com )
{
  FILE* fd;
  fd = fopen( MIDI_FILE_OUT, "a+" );

  if ( fd != NULL )
  {
    fprintf( fd, "    {0x%02X,0x%02X, 0x%04X, 0x%02X},\n",   com.command  , com.channel , com.deltaTime, com.noteNumber );
    fclose( fd );
  }
}

void printBeginCommand()
{
  FILE* fd;
  fd = fopen( MIDI_FILE_OUT, "a+" );

  if ( fd != NULL )
  {
    fprintf( fd, "{  \n" );
    fclose( fd );
  }
}

void printEndCommand()
{
  FILE* fd;
  fd = fopen( MIDI_FILE_OUT, "r+" );

  if ( fd != NULL )
  {
    fseek( fd, -2, SEEK_END );
    fprintf( fd, "\n};\n\n" );
    fclose( fd );
  }
}

void printEmpty()
{
  FILE* fd = fopen( MIDI_FILE_OUT, "a+" );

  if ( fd != NULL )
  {
    fprintf( fd, "    0X00  " );
    fclose( fd );
  }
}

void printCommandList( uint8_t commandCount )
{
  FILE* fd;
  fd = fopen( MIDI_FILE_OUT, "a+" );

  if ( fd != NULL )
  {
    fprintf( fd, "command* commandList[] = {\n" );
    uint8_t index = 0;
    for ( ; index < commandCount - 1; index++ )
    {

      fprintf( fd, "    track%d,\n", index + 1 );
    }
    if ( index < commandCount )
    {
      fprintf( fd, "    track%d\n", index + 1 );
    }
    fprintf( fd, "};\n\n" );
    fclose( fd );
  }
}

void printChunkList( midiTracks* tracks )
{
  FILE* fd;
  fd = fopen( MIDI_FILE_OUT, "a+" );

  if ( fd != NULL )
  {
    fprintf( fd, "trackChunk trackList[] = {\n" );
    uint8_t index = 0;
    for ( ; index < tracks->numTracks - 1; index++ )
    {
      fprintf( fd, "    {0x%02X, 0x%04X, 0x%02X, 0x%02X, 0x%02X},\n", 0x00, tracks->tracks[index].size.size, 0x00, 0x00,
          0x00 );
    }
    if ( index < tracks->numTracks )
    {
      fprintf( fd, "    {0x%02X, 0x%04X, 0x%02X, 0x%02X, 0x%02X}\n", 0x00, tracks->tracks[index].size.size, 0x00, 0x00,
          0x00 );
    }
    fprintf( fd, "};\n\n" );
    fclose( fd );
  }
}

void printMidi( uint8_t trackCount, uint16_t ppqn )
{
  FILE* fd;
  fd = fopen( MIDI_FILE_OUT, "a+" );

  if ( fd != NULL )
  {
    fprintf( fd, "midiTracks midi = { 0X%02X , trackList, commandList, 0X%04X};\n", trackCount, ppqn );
    fclose( fd );
  }
}

int main( int argc, char **argv )
{

  FILE* fd = fopen( MIDI_FILE_OUT, "w" );
  if ( fd != NULL )
  {
    fclose( fd );
  }

  //Create includes
  printPragmaBegin();
  printInclude();

  headerChunk* foo = (headerChunk*) malloc( sizeof(headerChunk) );

  convertMidi( argc, argv, foo );

  printPragmaEnd();
  return ( 0 );
}
