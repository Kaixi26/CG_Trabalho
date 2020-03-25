import Control.Monad
import Control.Monad.Tools
import System.Random

data Scene = Scene Sun
data Sun = Sun Name Radius Model [RelativePlanet]
data Planet = Planet Name Radius Model
data Moon = Moon Name Radius Model

newtype RelativeMoon = RelativeMoon { getRelativeMoon :: (Moon, Distance, Rotation) }
newtype RelativePlanet = RelativePlanet { getRelativePlanet :: (Planet, Distance, Rotation, [RelativeMoon]) }
type Name = String
type Radius = Float
type Distance = Float
type Rotation = (Float, Float, Float, Float)
type Model = String

scene :: IO Scene
scene = do
    [ap0,ap1,ap2,ap3,ap4,ap5,ap6,ap7,ap8] <- replicateM 9 (randomRIO (0.0::Float, 360.0::Float))
    return $ Scene (Sun "SUN" 1 "sphere.3d" [
        RelativePlanet ((Planet "MERCURY"   0.15    "sphere.3d"), 2     , (0,1,0,ap0), [])
      , RelativePlanet ((Planet "VENUS"     0.25    "sphere.3d"), 4     , (0,1,0,ap1), [])
      , RelativePlanet ((Planet "EARTH"     0.30    "sphere.3d"), 6     , (0,1,0,ap2), [ 
          RelativeMoon ((Moon   "MOON"      0.05    "sphere.3d"), 0.5   , (0,0,1,0))
          ])
      , RelativePlanet ((Planet "MARS"      0.20    "sphere.3d"), 9     , (0,1,0,ap3), [])
      , RelativePlanet ((Planet "JUPITER"   0.70    "sphere.3d"), 12    , (0,1,0,ap4), [
          RelativeMoon ((Moon   "EUROPA"    0.05    "sphere.3d"), 1     , (0,1,0,0))
        , RelativeMoon ((Moon   "LO"        0.10    "sphere.3d"), 1     , (0,1,0,90))
        , RelativeMoon ((Moon   "CALLISTO"  0.15    "sphere.3d"), 1     , (0,1,0,180))
        , RelativeMoon ((Moon   "EUROPA"    0.20    "sphere.3d"), 1     , (0,1,0,270)) 
        ])
      , RelativePlanet ((Planet "SATURN"    0.60    "sphere.3d"), 15    , (0,1,0,ap5), [
          RelativeMoon ((Moon   "MIMAS"     0.05    "sphere.3d"), 2     , (0,1,0,0))
        , RelativeMoon ((Moon   "ENCELADUS" 0.07    "sphere.3d"), 2     , (0,1,0,30))
        , RelativeMoon ((Moon   "THETHYS"   0.09    "sphere.3d"), 2     , (0,1,0,60))
        , RelativeMoon ((Moon   "DIONE"     0.11    "sphere.3d"), 2     , (0,1,0,90))
        , RelativeMoon ((Moon   "LAPETUS"   0.13    "sphere.3d"), 2     , (0,1,0,120))
        , RelativeMoon ((Moon   "RHEA"      0.17    "sphere.3d"), 2     , (0,1,0,150))
        , RelativeMoon ((Moon   "TITAN"     0.20    "sphere.3d"), 2     , (0,1,0,180))
      ])
      , RelativePlanet ((Planet "URANUS"    0.50    "sphere.3d"), 18    , (0,1,0,ap6), [
          RelativeMoon ((Moon   "MIRANDA"   0.05    "sphere.3d"), 1     , (0,1,0,60))
        , RelativeMoon ((Moon   "ARIEL"     0.01    "sphere.3d"), 1     , (0,1,0,120))
        , RelativeMoon ((Moon   "UMBRIEL"   0.12    "sphere.3d"), 1     , (0,1,0,180))
        , RelativeMoon ((Moon   "OBERON"    0.16    "sphere.3d"), 1     , (0,1,0,220))
        , RelativeMoon ((Moon   "TITANIA"   0.20    "sphere.3d"), 1     , (0,1,0,280))
      ])
      , RelativePlanet ((Planet "NEPTUNE"   0.40    "sphere.3d"), 21    , (0,1,0,ap7), [
          RelativeMoon ((Moon   "TRITON"    0.10    "sphere.3d"), 1     , (0,1,0,0))

      ])
      , RelativePlanet ((Planet "PLUTO"     0.1     "sphere.3d"), 24    , (0,1,0,ap8), [
          RelativeMoon ((Moon   "CHARON"    0.10    "sphere.3d"), 3     , (0,0,0,0))
      ]) ])

testMoon = Moon "Test" 3 "sphere.3d"
testRelativeMoon = RelativeMoon (testMoon, 10, (0,1,0,0))
testPlanet = Planet "TestPlanet" 20 "sphere.3d"
testRelativePlanet = RelativePlanet (testPlanet, 20, (0,1,0,0), [testRelativeMoon, testRelativeMoon])
testSun = Sun "Sun" 30 "sphere.3d" [testRelativePlanet, testRelativePlanet]

class XMLable a where
    toXML :: a -> String

instance XMLable Moon where
    toXML (Moon name rad model) = concat [
        "<group>\n"
      , "\t<scale ", "X=\"", show rad, "\" Y=\"", show rad, "\" Z=\"", show rad, "\"/>\n"
      , "\t<models> <model file=\"", model, "\" /> </models>\n"
      , "</group>\n"
        ]

instance XMLable RelativeMoon where
    toXML (RelativeMoon (moon@(Moon name _ _), dist, (axisX, axisY, axisZ, angle))) = concat $ [
        "<!-- ", name, " -->\n"
      , "<group>\n"
      , "\t<rotate axisX=\"", show axisX, "\" axisY=\"", show axisY, "\" axisZ=\"", show axisZ, "\" angle=\"", show angle, "\"/>\n"
      , "\t<translate X=\"", show dist, "\"/>\n"
      , ( unlines . map ((:)'\t') . lines . toXML) moon
      , "</group>\n"]


instance XMLable Planet where
    toXML (Planet name rad model) = concat $ [
        "<!-- ", name, " -->\n"
      , "<group>\n"
      , "\t<group>\n"
      , "\t\t<scale ", "X=\"", show rad, "\" Y=\"", show rad, "\" Z=\"", show rad, "\"/>\n"
      , "\t\t<models> <model file=\"", model, "\" /> </models>\n"
      , "\t</group>\n"
      , "</group>\n"]

instance XMLable RelativePlanet where
    toXML (RelativePlanet (planet@(Planet name _ _), dist, (axisX, axisY, axisZ, angle), moons)) = concat $ [
        "<!-- ", name, " AND MOONS", "-->\n"
      , "<group>\n"
      , "\t<rotate axisX=\"", show axisX, "\" axisY=\"", show axisY, "\" axisZ=\"", show axisZ, "\" angle=\"", show angle, "\"/>\n"
      , "\t<translate X=\"", show dist, "\"/>\n"
      , "\t<rotate axisX=\"", show (-axisX), "\" axisY=\"", show (-axisY), "\" axisZ=\"", show (-axisZ), "\" angle=\"", show angle, "\"/>\n"
      , (unlines . map ((:)'\t') . lines . toXML) planet
      , (concat . map (unlines . (map ((:)'\t')) . lines . toXML)) moons
      , "</group>\n"]


instance XMLable Sun where
    toXML (Sun name radius model planets) = concat $ [
        "<!-- ", name, "-->\n"
      , "<group>\n"
      , "\t<scale ", "X=\"", show radius, "\" Y=\"", show radius, "\" Z=\"", show radius, "\"/>\n"
      , "\t\t<models> <model file=\"", model, "\" /> </models>\n"
      , "</group>\n"
      , (concat . map (unlines . (map ((:)'\t')) . lines . toXML)) planets ]

instance XMLable Scene where
    toXML (Scene sun) = concat $ [
        "<scene>\n"
      , (unlines . map ((++) "\t") . lines . toXML) sun
      , "</scene>\n" ]

main = (liftM toXML scene) >>= putStr