[@bs.module "react-responsive-carousel"] external carousel: ReasonReact.reactClass = "Carousel";

[@bs.deriving abstract]
type jsProps = {
  showArrows:bool,
  showThumbs:bool,
  emulateTouch:bool,
  showIndicators:bool,
};


let make = (children) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=carousel,
    ~props=jsProps(
      ~showArrows=true,
      ~showThumbs=false,
      ~emulateTouch=true,
      ~showIndicators=false),
    children,
  );
